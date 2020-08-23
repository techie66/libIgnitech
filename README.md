<!-- vim: syntax=Markdown -->
# libIgnitech
Library to read information from the Ignitech TCIP-4 ignition controller (v88).
Ignitech S.R.O. is not affiliated with this software https://www.ignitech.cz

Just saying, this would be easier for me if someone else did it. :-P

## Requirements
Automake
Autoconf
Libtool
C++ compiler


## Installation
INSTALL contains modified generic GNU install instructions.

Quick installation: `./autogen.sh && ./configure && make && sudo make install`

This is made to be a shared library, so under Linux you may need to do `sudo ldconfig` after `sudo make install`

To make examples do `make examples`. You will get two files "ignitech_example_dyn" and "ignitech_example" the dyn version links to the installed library, the other does not. In order for make examples to not fail, you probably have to run `make install' first.

make git-clean will delete all auto generated files and leave just the files that would normally be in the git repository.

# TODO
- [ ] API
- [ ] Checksum checking
- [ ] Support for v96
- [x] CLI Example
- [x] Communication setup
- [x] querying controller
- [x] Parsing response
