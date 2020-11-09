#/bin/sh
./example/tcip4_emulator --quiet --firmware-version 88 --sweep SENSORMV --pipe-name sensormv-tty &
PID=$!
while [ ! -e sensormv-tty ]
do
	sleep 0.1
done
./check/sensormv
ret88=$?
wait $PID
echo V88: $ret88
./example/tcip4_emulator --quiet --firmware-version 96 --sweep SENSORMV --pipe-name sensormv-tty &
PID=$!
while [ ! -e sensormv-tty ]
do
	sleep 0.1
done
./check/sensormv
ret96=$?
wait $PID
echo V96: $ret96

if [[ $ret88 -eq 0 && $ret96 -eq 0 ]]
then
	exit 0
else
	exit 1
fi