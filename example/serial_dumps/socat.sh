#!/bin/bash
socat /dev/ignitech,raw,echo=0 SYSTEM:'tee in.bin |socat - "PTY,link=./virtual-tty,raw,echo=0,waitslave" |tee out.bin'&
#socat tcp:localhost:2000,keepalive,forever SYSTEM:'tee in.bin |socat - "PTY,link=./virtual-tty,raw,echo=0,waitslave" |tee out.bin'&
#socat tcp:localhost:2000,keepalive,forever PTY,link=./virtual-tty,raw,echo=0,waitslave&
