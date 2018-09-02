# Compilation notes

There are two hardware versions. The development version is Version 1
and the release version is version 2.

* So for all users: uncomment **VERSION2** in _handcontroller.ino_ if
it is commented out below

This version uses _EnableInterrupt_ library that is available for
download here
[EnableInterrupt](https://github.com/GreyGnome/EnableInterrupt/wiki/Usage)

Simply copy
* _EnableInterrupt.h_ file 
* and the subdirectory _utility_
to the same directory where _hand_controller.ino_ resides.

Then the _hand_controller.ino_ source file should compile. I have
successfully compiled it with _Arduino_ 1.8.5.

