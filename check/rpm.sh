#/bin/sh
./example/tcip4_emulator --quiet --firmware-version 88 --sweep RPM --pipe-name rpm-tty &
PID=$!
while [ ! -e rpm-tty ]
do
	sleep 0.1
done
./check/rpm
ret88=$?
wait $PID
echo V88: $ret88
./example/tcip4_emulator --quiet --firmware-version 96 --sweep RPM --pipe-name rpm-tty &
PID=$!
while [ ! -e rpm-tty ]
do
	sleep 0.1
done
./check/rpm
ret96=$?
wait $PID
echo V96: $ret96
if [[ $ret88 -eq 0 && $ret96 -eq 0 ]]
then
	exit 0
else
	exit 1
fi