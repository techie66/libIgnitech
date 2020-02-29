#!/bin/bash
socat PTY,link=./virtual-tty,raw,echo=0 EXEC:./tcip4_emulator&
