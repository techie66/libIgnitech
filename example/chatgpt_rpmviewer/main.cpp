/*
 * rpm_viewer - A simple Qt6 GUI to view RPM from an Ignitech TCIP-4
 * ignition controller via libIgnitech.
 *
 * Build (requires Qt6 and libIgnitech installed):
 *   mkdir build && cd build
 *   cmake .. && make
 *
 * Run:
 *   ./rpm_viewer
 *
 * Copyright (C) 2024  GPL-2.0-or-later
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QDir>
#include <QFont>
#include <QFrame>
#include <QSerialPortInfo>
#include <QPainter>
#include <QPainterPath>
#include <deque>

#include "../src/ignitech.h"   /* libIgnitech header */

/* Lightweight custom-painted sparkline. Keeps a ring buffer of the most
 * recent RPM samples and draws them as a polyline. No Qt Charts needed. */
class RpmSparkline : public QWidget {
    Q_OBJECT
public:
    explicit RpmSparkline(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumHeight(120);
    }

    void push(int rpm) {
        m_samples.push_back(rpm);
        while (m_samples.size() > kMaxSamples)
            m_samples.pop_front();
        update();
    }

    void clear() {
        m_samples.clear();
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        const QRect r = rect().adjusted(8, 8, -8, -8);
        p.fillRect(r, QColor(245, 248, 252));
        p.setPen(QColor(220, 225, 232));
        p.drawRect(r);

        if (m_samples.size() < 2) {
            p.setPen(QColor(170, 175, 185));
            p.drawText(r, Qt::AlignCenter, "awaiting data…");
            return;
        }

        int lo = *std::min_element(m_samples.begin(), m_samples.end());
        int hi = *std::max_element(m_samples.begin(), m_samples.end());
        if (hi - lo < 100) {            // give a little vertical breathing room
            int pad = (100 - (hi - lo)) / 2;
            lo = qMax(0, lo - pad);
            hi += pad;
        }
        if (hi == lo) hi = lo + 1;      // avoid divide-by-zero on flat line

        const double sx = double(r.width()) / (kMaxSamples - 1);
        const double sy = double(r.height()) / (hi - lo);

        QPainterPath path;
        int i = 0;
        for (int s : m_samples) {
            double x = r.left() + (i + (kMaxSamples - int(m_samples.size()))) * sx;
            double y = r.bottom() - (s - lo) * sy;
            if (i == 0) path.moveTo(x, y);
            else       path.lineTo(x, y);
            ++i;
        }

        // fill under the curve
        QPainterPath fill = path;
        double lastX = r.left() + (int(m_samples.size() - 1) +
                        (kMaxSamples - int(m_samples.size()))) * sx;
        fill.lineTo(lastX, r.bottom());
        fill.lineTo(r.left() + (kMaxSamples - int(m_samples.size())) * sx, r.bottom());
        fill.closeSubpath();
        p.fillPath(fill, QColor(80, 140, 220, 40));

        // line
        p.setPen(QPen(QColor(40, 110, 200), 2));
        p.drawPath(path);

        // min/max labels
        p.setPen(QColor(120, 130, 145));
        p.setFont(QFont("Sans", 8));
        p.drawText(r.adjusted(4, 2, -4, -r.height() + 16),
                   Qt::AlignTop | Qt::AlignLeft, QString::number(hi));
        p.drawText(r.adjusted(4, r.height() - 16, -4, -2),
                   Qt::AlignBottom | Qt::AlignLeft, QString::number(lo));
    }

private:
    static constexpr int kMaxSamples = 60; // ~30 s of history at 2 Hz
    std::deque<int> m_samples;
};

class RpmViewer : public QWidget {
    Q_OBJECT
public:
    explicit RpmViewer(QWidget *parent = nullptr) : QWidget(parent), m_ignitech(nullptr) {
        setWindowTitle("Ignitech RPM Viewer");
        resize(440, 420);

        auto *root = new QVBoxLayout(this);
        root->setContentsMargins(20, 20, 20, 20);
        root->setSpacing(12);

        /* --- Port selection row --- */
        auto *portRow = new QHBoxLayout;
        m_portCombo = new QComboBox(this);
        m_portCombo->setEditable(true);          // allow manual entry (virtual/pty ports)
        m_portCombo->setInsertPolicy(QComboBox::NoInsert);
        m_portCombo->setMinimumWidth(220);
        refreshPorts();

        auto *refreshBtn = new QPushButton("Refresh", this);
        m_connectBtn = new QPushButton("Connect", this);
        m_connectBtn->setCheckable(true);

        portRow->addWidget(new QLabel("Serial port:", this));
        portRow->addWidget(m_portCombo, 1);
        portRow->addWidget(refreshBtn);
        portRow->addWidget(m_connectBtn);
        root->addLayout(portRow);

        /* --- Separator --- */
        auto *sep = new QFrame(this);
        sep->setFrameShape(QFrame::HLine);
        sep->setFrameShadow(QFrame::Sunken);
        root->addWidget(sep);

        /* --- RPM readout --- */
        m_rpmLabel = new QLabel("----", this);
        m_rpmLabel->setAlignment(Qt::AlignCenter);
        QFont f = m_rpmLabel->font();
        f.setPointSize(56);
        f.setBold(true);
        m_rpmLabel->setFont(f);

        m_caption = new QLabel("RPM", this);
        m_caption->setAlignment(Qt::AlignCenter);
        QFont cf = m_caption->font();
        cf.setPointSize(14);
        m_caption->setFont(cf);

        root->addWidget(m_rpmLabel);
        root->addWidget(m_caption);

        /* --- Sparkline graph --- */
        m_sparkline = new RpmSparkline(this);
        root->addWidget(m_sparkline);

        m_statusLabel = new QLabel("Disconnected", this);
        m_statusLabel->setAlignment(Qt::AlignCenter);
        m_statusLabel->setStyleSheet("color: #888;");
        root->addWidget(m_statusLabel);
        root->addStretch(1);

        /* --- Polling timer (read_async is non-blocking) --- */
        m_timer = new QTimer(this);
        m_timer->setInterval(100);   // 10 Hz; read_async returns IGN_AGAIN if no packet yet
        connect(m_timer, &QTimer::timeout, this, &RpmViewer::poll);

        /* --- Signals --- */
        connect(refreshBtn, &QPushButton::clicked, this, &RpmViewer::refreshPorts);
        connect(m_connectBtn, &QPushButton::toggled, this, &RpmViewer::toggleConnection);
    }

    ~RpmViewer() override {
        disconnectPort();
    }

private slots:
    void refreshPorts() {
        m_portCombo->clear();
        const auto ports = QSerialPortInfo::availablePorts();
        for (const auto &p : ports) {
            QString label = p.portName();
            QString dev = p.systemLocation();
            if (!dev.startsWith("/dev/"))
                dev = "/dev/" + p.portName();
            m_portCombo->addItem(label + "  (" + dev + ")", dev);
        }
        if (m_portCombo->count() == 0) {
            QDir dev("/dev");
            const QStringList filters = {"ttyUSB*", "ttyACM*", "ttyS*", "ttyAMA*"};
            QStringList entries = dev.entryList(filters, QDir::System);
            entries.sort();
            for (const auto &e : entries)
                m_portCombo->addItem(e, "/dev/" + e);
        }
        m_portCombo->addItem("/tmp/virtual-tty", "/tmp/virtual-tty");
        m_portCombo->addItem("/tmp/virtual-tty2", "/tmp/virtual-tty2");
        m_portCombo->insertItem(0, "— select port —", QString());
        m_portCombo->setCurrentIndex(0);
    }

    void toggleConnection(bool connected) {
        if (connected) {
            QString dev = m_portCombo->currentData().toString();
            if (dev.isEmpty())                  // use manually typed path (e.g. pty)
                dev = m_portCombo->currentText().trimmed();
            if (dev.isEmpty()) {
                QMessageBox::warning(this, "No port",
                    "Please select a serial port first.");
                m_connectBtn->setChecked(false);
                return;
            }
            try {
                m_ignitech = new IGNITECH(dev.toLocal8Bit().constData());
            } catch (const std::exception &e) {
                QMessageBox::critical(this, "Error",
                    QString("Could not open %1:\n%2").arg(dev, e.what()));
                m_ignitech = nullptr;
                m_connectBtn->setChecked(false);
                return;
            } catch (...) {
                QMessageBox::critical(this, "Error",
                    QString("Could not open %1.").arg(dev));
                m_ignitech = nullptr;
                m_connectBtn->setChecked(false);
                return;
            }
            m_connectBtn->setText("Disconnect");
            m_portCombo->setEnabled(false);
            m_statusLabel->setText("Connected — reading…");
            m_statusLabel->setStyleSheet("color: #2a7;");
            m_sparkline->clear();
            m_timer->start();
            poll();
        } else {
            disconnectPort();
        }
    }

    void poll() {
        if (!m_ignitech) return;
        ignitech_t data{};
        IGN_async_status st = m_ignitech->read_async(data);
        if (st == IGN_AGAIN) return;            // no complete packet yet — keep waiting
        if (st == IGN_ERR || st == IGN_BAD) {
            m_rpmLabel->setText("ERR");
            m_statusLabel->setText(QString("Read error: %1").arg(int(st)));
            m_statusLabel->setStyleSheet("color: #c33;");
            return;
        }
        // IGN_SUC
        m_rpmLabel->setText(QString::number(data.rpm));
        m_sparkline->push(data.rpm);
        m_statusLabel->setText(QString("OK   battery %1 V")
            .arg(data.battery_mV / 1000.0, 0, 'f', 1));
        m_statusLabel->setStyleSheet("color: #2a7;");
    }

private:
    void disconnectPort() {
        m_timer->stop();
        if (m_ignitech) {
            delete m_ignitech;
            m_ignitech = nullptr;
        }
        m_rpmLabel->setText("----");
        m_sparkline->clear();
        m_statusLabel->setText("Disconnected");
        m_statusLabel->setStyleSheet("color: #888;");
        m_portCombo->setEnabled(true);
        m_connectBtn->setText("Connect");
        m_connectBtn->setChecked(false);
    }

    QComboBox *m_portCombo;
    QPushButton *m_connectBtn;
    QLabel *m_rpmLabel;
    QLabel *m_caption;
    QLabel *m_statusLabel;
    RpmSparkline *m_sparkline;
    QTimer *m_timer;
    IGNITECH *m_ignitech;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setApplicationName("rpm_viewer");
    RpmViewer w;
    w.show();
    return app.exec();
}

#include "main.moc"
