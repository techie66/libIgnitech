#/bin/sh
../example/tcip4_emulator --quiet --firmware-version 88 --sweep SENSORMV &
./sensormv
ret88=$?
../example/tcip4_emulator --quiet --firmware-version 96 --sweep SENSORMV &
./sensormv
ret96=$?

if [[ $ret88 -eq 0 && $ret96 -eq 0 ]]
then
	exit 0
else
	exit 1
fi
