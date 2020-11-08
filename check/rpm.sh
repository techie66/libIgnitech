#/bin/sh
#../example/tcip4_emulator --quiet --firmware-version 88 --sweep RPM &
#./rpm
#ret88=$?
../example/tcip4_emulator --quiet --firmware-version 96 --sweep RPM &
./rpm
ret96=$?
if [ $ret88 -eq 0 && $ret96 -eq 0 ]
then
	exit 0
else
	exit 1
fi
