#/bin/sh
../example/tcip4_emulator --quiet --firmware-version 88 --sweep SENSORMV &
./sensormv
exit $?
