# VenusOsTFT35
Instructions for using cheap TFT35 LCD Touchscreen with VenusOS from Victron Energy in a RaspberryPi

## Introduction

For those who don't know, VenuOS is a Linux distribution for several devices from Victron Energy. These devices serve as the "brain" of a Photovoltaic system, in charge of talking with all the componets of this PV System, like: inverters, chargers or battery BMS's among others. Such OS is well-known to be open to download and installed not only in Victron devices (Color Control, Cerbo, etc.), but also into Raspberry Pi devices. This degree of freedom and the fact that Victron has GitHub pages with drivers for third-party HW, Wiki with tutorials and a huge community behind the scenes, makes Victron the brand of choice of many PV enthusiasts/tinkerers like me. Huge thanks and thumbs up to Victron! 

As I mentioned before, the Victron's brain of a PV System is made up of the HW (Victron branded or Raspberry PI) and SW - VenusOS. For interacting with the OS UI you have two ways: by web or by touchscreen. Using any ethernet capable device such as Mobile Phone, PC or a Tablet, you can get into VenusOS UI by accessing http://venus.local on your local network. This requires you to connect your device to your LAN. On the other hand, you can interact with the UI without the requirement of a LAN by using a Touchscreen. Victron has its own High Quality-High Priced Touchscreen but ... we want to have this option at the very lowest price ... And this, folks, is where this repo is for. Using a Raspberry Pi with a cheap TFT 3.5" touchscreen display.

## The Cheap LCD Touchscreen for the Raspberry Pi

The Cheap Touchscreen LCDs look like:
[embed image here]
http://www.lcdwiki.com/images/4/44/MPI3501-001.jpg

The one I used is a Kuman 3.5" Inch 480x320 TFT LCD Touch Screen. This comes with a LCD driver ILI9486 and a resistive touch controller XPT2046. There may be variations on the driver and touch controller, but these two are very common. As far as I know, there are other brands out there like the WaveShare one, that are very similar if not identical.

## The issue and the problem

### The issue (backlight control)

Well, the issue is that these cheap touchscreens **do not come with a backlight control** ... So, the screen is on all the time, consuming 75~100mA for the backlight.

To implement this, I designed a little circuit to have ON/OFF control on the backlight, which requires 3 resistors, a transistor and minimal soldering skills.

### The problem (Framebuffers)
There is one major disadvantage that prevent these type of screens to work easily on the latest versions of VenusOS. It is related to Framebuffers.

[In the next lines, there may be some inaccuracies]
Linux works with Framebuffers to show up screen content onto your actual monitor. If you have several graphic cards in your machine, then you will have several framebuffers, one for each graphical device. Earlier versions of Venus (do not really know when this changed) only had two framebuffers. So, when executing this on the raspberry :
```
cat /proc/fb
```
The output was something like this:
```
0 BCM2708 FB
1 fb_ili9486
```
Here framebuffer 0 corresponded to Broadcom GPU. The fb no. 1 was from the cheap tft screen. So you had to do some little mapping into the /u-boot/cmdline.txt, by appending

```
echo fbcon=map:10 >> /u-boot/cmdline.txt
```

But ... since [some FW Version] fb list extended to:
```
0 simple
1 BCM2708 FB
2 fb_ili9486
```
and the easy fbcon=map:210 into /u-boot/cmdline.txt didn't do the job :(

So, if no remapping is possible there were to possible solutions:

1 - Change order of fb inside Linux config to first use fb_ili9486 instead of 0 simple ---> but this is strictly prohibited by the OS.
2 - Find a way to duplicate the content being streamed to fb 0 (which indirectly is using BCM2708) into 2 fb_ili9486 --> **SOLUTION!! Use program called fbcp.**


## Backlight control

### Hardware

In order to implement hw modification, you will have to find the limiting resistor for the LED backlight string. In my case is R5 - 2.2 ohm. So, you need to desolder it and install the next circuit, like shown:

For R2, choose values between 1k and 1k5 (for sufficient base current) and for R3 values between 10k and 56k (for correct base pull-down)

![image](https://user-images.githubusercontent.com/35175513/179475164-faaac4ad-1c70-4f27-9ef2-896fc098d743.png)

![image](https://user-images.githubusercontent.com/35175513/179475322-ea8da534-c25f-416f-a57a-051a14aaf7e6.png)

![image](https://user-images.githubusercontent.com/35175513/179475387-592781b8-a9b4-4365-a961-9fae18553d0e.png)

I've chosen RPi GPIO18(PIN12 of header) because it is also capable of PWM and 2N3904 is fine upto 100MHz. So, in case of PWM dimming everything will be fine. But in this solution I have only implemented a ON-OFF solution, for simplicity.

### Software

In terms of sw, I had to write a small C program to manage backlight. The programs controls the GPIO port and automatically shuts the backlight of after any given time value in seconds without touching the display and turns on after touching the screen when backlight is off.

For this, I used WiringPi library and system calls for the touchscreen input device. See backlightCtrl.c for more details.

This little program takes two arguments:

Arg1: input device - Example: /dev/input/touchscreen0
Arg2: backlight shutdown timer in seconds - Example: 45

Usage example:

```
backlightCtrl /dev/input/touchscreen0 45
```

This command will be lauched by default in a rc.local script I will provide. So, everytime the raspberry boots up, this programm will take care of backlight.

