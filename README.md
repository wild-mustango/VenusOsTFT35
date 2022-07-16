# VenusOsTFT35
Instructions for using cheap TFT35 LCD Touchscreen with VenusOS from Victron Energy in a RaspberryPi

Introduction

For those who don't know, VenuOS is a Linux distribution for several devices from Victron Energy. These devices serve as the "brain" of a Photovoltaic system, in charge of talking with all the componets of this PV System, like: inverters, chargers or battery BMS's among others. Such OS is well-known to be open to download and installed not only in Victron devices (Color Control, Cerbo, etc.), but also into Raspberry Pi devices. This degree of freedom and the fact that Victron has GitHub pages with drivers for third-party HW, Wiki with tutorials and a huge community behind the scenes, makes Victron the brand of choice of many PV enthusiasts/tinkerers like me. Huge thanks and thumbs up to Victron! 

Going back to the roots of this repo. As I mentioned before, the Victron's brain of a PV System is made up of the HW (Victron branded or Raspberry PI) and SW - VenusOS. For interacting with the OS UI you have two ways: by web or by touchscreen. Using any ethernet capable device such as Mobile Phone, PC or a Tablet, you can get into VenusOS UI by accessing http://venus.local on your local network. This requires you to connect your device to your LAN. On the other hand, you have interact with the UI without the requirement of a LAN by using a Touchscreen. Victron has its own High Quality-High Priced Touchscreen but ... we want to have this option at the very lowest price ... And this, folks, is where this repo is for. Using a Raspberry Pi with a cheap TFT 3.5" touchscreen display.

The Cheap LCD Touchscreen


