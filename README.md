<!-- vim: syntax=Markdown -->
# libIgnitech
Library to read information from the Ignitech TCIP-4 ignition controller (v88).
Ignitech S.R.O. is not affiliated with this software https://www.ignitech.cz

Just saying, this would be easier for me if someone else did it. :-P

## Requirements
Libtool
C++ compiler


## Installation
INSTALL contains modified generic GNU install instructions.

```
### Quick installation: 
	./configure && make && sudo make install
```

This is made to be a shared library, so under Linux you may need to do `sudo ldconfig` after `sudo make install`
Configure errors may require `./autogen.sh`

### Examples
To make examples do:
```
	make examples
```
You will get "ignitech_example".

To make a dynmically linked example:
```
	cd example
	make example_dyn 
```
In order for make example_dyn to not fail, you have to run `make install' first.

### Other targets
make git-clean will delete all autotools generated files and leave just the files that would normally be in the git repository.
make uninstall works as expected.

# TODO
- [ ] API
- [ ] Checksum checking
- [ ] Support for v96
- [x] CLI Example
- [x] Communication setup
- [x] querying controller
- [x] Parsing response
