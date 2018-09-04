# A simple hand controller for Astro-Physics mounts

by C.Y. Tan 2018

![Simple Astro-Physics Hand controller](https://github.com/cytan299/ap_hand_controller/blob/master/pics/closed_small.jpg)

It is well-known, that the new GTOCP4 control box can be controlled
with a phone or a pad. However, I really like the feel of real buttons
in a controller rather than using "virtual" buttons on a phone or pad.
This means that the only solution that Astro-Physics (AP) has for people like
me is to buy their AP keypad.

If I had the AP keypad, I will mainly use it for Polar aligning with
PoleMaster and for recalibrating (which is the same as syncing for all
other mounts). After polar alignment and recalibrating, PHD2 will be
used for guiding the scope. Therefore, the AP keypad will only be used
at the start of my imaging session, and no more. Thus, it is hard for
me to justify spending $1000 for an AP keypad when I will hardly use.
I had to figure out a solution.

And here's my solution:
* I have built a simple hand controller for my Mach1GTO. Its purpose
is **not** to replace the AP keypad because it does not initialize the
CP4, have any star database or goto capability etc. My goal is to just
move the mount for calibrating the mount to a star and be compatible
with PoleMaster.

![Hand controller connected to the GTOCP4 ](https://github.com/cytan299/ap_hand_controller/blob/master/pics/IMG_3213.jpg)

## Design

The goal of my hand controller is to be low cost (less than $200). The
simplest way I know how to do this is to base it on an
[Arduino controller](https://www.sparkfun.com/products/12640). The
other low cost components are

* A small 64x48
  [OLED display](https://www.ebay.com/itm/0-66-OLED-Display-Module-7pin-64x48-Screen-SPI-I2C-3-3-5V-for-Arduino-AVR-STM32/311968961516?_trkparms=aid%3D111001%26algo%3DREC.SEED%26ao%3D1%26asc%3D51025%26meid%3Dd4485726d0e34c16a699dcc92951db6e%26pid%3D100675%26rk%3D5%26rkt%3D15%26sd%3D323141166313%26itm%3D311968961516&_trksid=p2481888.c100675.m4236&_trkparms=pageci%3Ada1189f9-3ac9-11e8-906e-74dbd1800d52%7Cparentrq%3Aa2cf6cb51620ab6ac78552f3fffc49d7%7Ciid%3A1)
  for showing the user options and the state of the controller.
* A [thumb joystick](https://www.sparkfun.com/products/9032) for
selecting menu items and for moving the telescope in RA and DEC.
* The hand controller will be connected to the same port that the AP
  keypad uses. 

## Support

This is unsupported hardware. Build at your own peril! :)

You can submit questions or bug reports using the
[issues](https://github.com/cytan299/ap_hand_controller/issues) tab on
the right and then click on **NEW**.

Have fun!

## Directory structure

* **howto** Assembly instructions for the hand controller.
* **ponoko** The files that are used for laser cutting the acrylic parts of
the design. These files can be sent directly to
[Ponoko](http://www.ponoko.com) for laser cutting.
* **bom** Bill of materials for one hand controller.
* **eagle** Eagle schematic and board files.
* **arduino** Source code for the hand controller.
* **pics** Photographs used in the README's and Wiki.
* **user manual** contains the hand controller user manual.

## Other information

More information can be found in he *README.md* files in each
directory and the [wiki](https://github.com/cytan299/ap_hand_controller/wiki/A-simple-hand-controller-for-Astro-Physics-mounts) of this project on github.

## Copyright
All the software, documentation, and hardware that I have written is
copyright 2018 C.Y. Tan.

All software is released under GPLv3

All documentation is released under:
* Creative Commons Attribution-ShareAlike 3.0 Unported License
* GNU Free Documentation License, Version 1.3
* Creative Commons Attribution-ShareAlike 4.0 International License

All hardware is released under CERN Hardware Open License v1.2



