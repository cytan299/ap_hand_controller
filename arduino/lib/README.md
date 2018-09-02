# Arduino libraries that are required for the hand controller

This directory contains all the libraries that are necessary for
building *hand_controller.ino*. Two of these libraries have to be installed in

*~/Documents/Arduino/libraries*

on a Mac. For Linux or a PC, you will have to install it in the
correct place that the Arduino compiler requires.

The _enableInterrupt_ library has to installed in the same
subdirectory where *hand_controller.ino* resides.

### Libraries that are zip'ed

These libraries are needed to compile *hand_controller.ino* which I
have zipped here for convenience.

* **MenuSystem.zip** is from [Arduino Menu System Library](http://www.jonblack.me/arduino-menu-system-library/)
* **SFE_MicroOLED.zip** is from
[Sparkfun Micro OLED Arduino Library](https://github.com/sparkfun/SparkFun_Micro_OLED_Arduino_Library)

The above files have to be unzipped and installed in
*~/Documents/Arduino/libraries*
on a Mac. For Linux or Windows, you will have to install it in the
correct place that the Arduino compiler requires.

The following file
* **enableinterrupt-1.0.0.zip** is from
[EnableInterrupt](https://github.com/GreyGnome/EnableInterrupt)

has to be unzipped and then simply copy
* _EnableInterrupt.h_ file 
* and the subdirectory _utility_
to the same directory where _hand_controller.ino_ resides.












