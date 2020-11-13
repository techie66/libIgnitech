#/bin/sh
echo "Starting emulator"
./example/tcip4_emulator --quiet --firmware-version 88 --sweep $2 --pipe-name $1-tty &
PID=$!
while [ ! -e $1-tty ]
do
	sleep 0.1
	echo "Waiting for pipe"
done
echo "starting check program"
./check/$1
ret88=$?
wait $PID
echo V88: $ret88
echo "Starting emulator"
./example/tcip4_emulator --quiet --firmware-version 96 --sweep $2 --pipe-name $1-tty &
PID=$!
while [ ! -e $1-tty ]
do
	sleep 0.1
	echo "Waiting for pipe"
done
echo "starting check program"
./check/$1
ret96=$?
wait $PID
echo V96: $ret96
if [[ $ret88 -eq 0 && $ret96 -eq 0 ]]
then
	exit 0
else
	exit 1
fi
