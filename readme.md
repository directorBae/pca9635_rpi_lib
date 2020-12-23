# PCA9635 Raspberry Pi Library (LED Driver)
This library provides a good interface to access the PCA9635 chip and provides a pallete to normalize the 
minimum and maximum brightness for different color LED's.  The example app will flash all of the output pins, 
then flash them one at a time in succession.  

To do:  Add user adjustable pallets. 

Copyright (c) 2019 Wade Ryan

## Advantages

1) Designed for LED’s.  No resitor required for most ultra bright LED's
2) Each individual LED brightness can be set via PWM settings
3) PWM output can be used with LN298
4) Comes in very compact TSSOP28 package
5) Cousin to pca 9685servo driver

## Disadvantages

1) No DIP package; Currently only comes in TSSOP28 package
2) More complex than mcp23017
3) No input channels (but is that really a disadvantage?)
3) No CLI (yet)
4) Not for use with individually Addressable LED's

## Requirements
On your Raspberry Pi, please use the raspi-config program to enable the I2C interface.
Then use the gpio command to make sure your i2c devices can be found.  

    $ sudo raspi-config
    $ gpio i2cd


## Install
To compile this library, navigate into the src folder and use the make utility to compile 
and install the library.

    $ cd [project folder]
    $ cd src
    $ make && sudo make install


## Compiling


Complie your applications using these command line arguments: -lwiringPi -lwiringPiADS1115rpi


## Example
To run the example program, nagaviate into the example folder and use make to compile the program. 

    $ cd ../example
    $ make 
    $ ./flashlight -a 0f
    $ initialization complete, pca9635 handle=4
    $ .0123456789abcdef
    $ .0123456789abcdef
    $ .0123456789abcdef
    $ (etc.)

## Datasheet

https://www.nxp.com/docs/en/data-sheet/PCA9635.pdf
