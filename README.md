<!-- vim: syntax=Markdown -->
# libIgnitech
Library to read information from the Ignitech TCIP-4 ignition controller (v88
or v96) via its proprietary serial protocol.
Currently supporting the folowing data:
 - RPM
 - Battery Voltage
 - Engine load (the unit sends measured sensor voltage and a converted value from either TPS or MAP)

Ignitech S.R.O. is not affiliated with this software https://www.ignitech.cz

Just saying, this would be easier for me if someone else did it. :-P

## Requirements
Libtool
C++ compiler

### Quick Installation
```
	git clone git@github.com:techie66/libIgnitech.git
	./configure
	make
	sudo make install
```

## Installation
INSTALL contains modified generic GNU install instructions.
```
	git clone git@github.com:techie66/libIgnitech.git
	./configure
	make
	make check
	sudo make install
```
### Automated checks
`make check` runs a lot of tests that can be very slow on minimal hardware (read, Raspberry Pi Zero) and may take hours to complete. Progress can be checked in the *.log files under the 'check' directory while the tests are running.

### Autotools versions
Sometimes different versions of autotools will complain and cause errors while running configure. If you have any unexplained errors, try running `./autogen.sh` from the main directory and try building again.

### Library installation
This is made to be a shared library. Normally the install process will take care of properly installing the library so it can be used, but sometimes (ie installation to a non-standard directory) programs will fail to run with an error like "unable to find libignitech.so.0" In that case you may need to do `sudo ldconfig` after `sudo make install`

### Examples
An example program is made automatically by make. It is located under the 'example' directory.
An easy way to try it out:
```
	./example/tcip4_emulator -f 96 &
	./example/ignitech_example
```

#### ignitech_example
This program is bare bones code to show how to use the library. The program is hard-coded to connect to the file 'virtual-tty' in the current directory and will read and display values. It is possible to symlink 'virtual-tty' to an actual serial port (eg /dev/ttyS0 or /dev/ttyUSB0) to read from a real device.

#### tcip4_emulator
This program is the workhorse of testing. It is only capable of responding to normal data requests (not programming requests). Usage information can be obtained by running `tcip4_emulator --help` or more detailed `tcip4_emulator --detailed-help`
It is very helpful while developing an application that uses the library since it removes the need for having an actual TCIP4 available for testing.

To make a dynmically linked example:
```
	cd example
	make example_dyn 
```
In order for make example_dyn to not fail, you have to run `make install` first.

### Other targets
make git-clean will delete all autotools generated files and leave just the files that would normally be in the git repository.
make uninstall works as expected.

# TODO
- [ ] API // Not yet stable
- [x] Checksum checking
- [x] Support for v96
- [x] CLI Example
- [x] Communication setup
- [x] querying controller
- [x] Parsing response
