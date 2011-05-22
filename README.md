uA Thermometer
==============

A simple project for an ATTiny13 microcontroller that implements an "analog" thermometer using a microampere meter.

It uses PWM to set the level on the meter. It is calibrated for the meter I had at hand, which goes from 0 to 50. Recalibration may be required for your meter.

Parts
=====

ATTiny13 microcontroller
DS18B20 1-wire thermometer
4.7k pullup for DS18B20
100k resistor for uA meter
2x3-pin ISP header for programming

Schematic
=========

Schematic for EAGLE PCB is in schematic.sch.

Connect ATTiny pin 8 to 5V, pin 4 to GND.
Connect DS18B20 middle leg to pin 2 (PB3), and the 4.7k resistor (the other end goes to 5V)
Connect DS18B20 5V and GND
Connect a 100k resistor to pin 5 (PB0). The other end connects to the live (red) wire on the uA-meter
Finally, connect the ground wire on the uA-meter to GND

Firmware
========

The most complex part of the firmware is the code to read the onewire temperature sensor. There is a good avr-gcc
library at http://gandalf.arubi.uni-kl.de/avr_projects/tempsensor/index.html

Problem is, the ATTiny13 has only 1k of flash memory, and the full onewire library is too big to use. The library
written by Peter Dannegger is based on onewire code by Peter Fleury. Using an earlier version of the code, I was able
to trim it down so that it would fit on the ATTiny13, and still give correct temperature readings. The original code
had no copyright notice, so I put in the one from the library.
