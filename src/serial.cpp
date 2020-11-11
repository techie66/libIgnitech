/*
    libIgnitech - Library to communicate with Ignitech TCIP-4 ignition controller
    Copyright (C) 2019-2020  Jacob Geigle

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "ignitech.h"
#include "serial.h"

int
IGNITECH::set_interface_attribs (int fd, int speed, int parity) {
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0) {
		perror("Error getting tty attr:");
	}

	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

/*
 *
 * FOR TCIP-4 stty output needs to be:
speed 57600 baud; line = 0;
min = 0; time = 0;
ignbrk -brkint -icrnl -imaxbel
-opost
-isig -icanon -iexten -echo noflsh

OR MAYBE
speed 57600 baud; line = 0;
intr = <undef>; quit = <undef>; erase = <undef>; kill = <undef>; eof = <undef>; start = <undef>; stop = <undef>; susp = <undef>;
rprnt = <undef>; werase = <undef>; lnext = <undef>; discard = <undef>; min = 1; time = 0;
ignbrk -brkint -icrnl -imaxbel
-opost -onlcr
-isig -icanon -iexten -echo -echoe -echok -echoctl -echoke

*/

	cfmakeraw(&tty);
	/* Specific for TCIP-4 */
	tty.c_lflag |= NOFLSH;
	tty.c_iflag |= IGNBRK;
	/* END TCIP-4 Specific */
	tty.c_cc[VMIN]  = 0;		// read doesn't wait for min bytes
	tty.c_cc[VTIME] = 5;		// 0.5 seconds read timeout
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag |= (CLOCAL | CREAD);
	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag |= parity;

	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		perror("Error setting tty attr:");
	}
	return 0;
}

void
set_blocking (int fd, int should_block) {
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0) {
		perror("Error getting tty attr:");
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;		// 0.5 seconds read timeout

	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		perror("Error setting tty attr:");
	}
}

