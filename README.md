# VenusOsTFT35
Instructions for using cheap TFT35 LCD Touchscreen with VenusOS from Victron Energy in a RaspberryPi

## Introduction

For those who don't know, VenuOS is a Linux distribution for several GX devices from Victron Energy. These GX devices serve as the "brain" of a Photovoltaic system, in charge of talking with all the componets of this PV System, like: inverters, chargers or battery BMS's among others. Such OS is well-known to be open to download and installed not only in Victron devices (Color Control, Cerbo, etc.), but also into Raspberry Pi devices. This degree of freedom and the fact that Victron has GitHub pages with drivers for third-party HW, Wiki with tutorials and a huge community behind the scenes, makes Victron the brand of choice of many PV enthusiasts/tinkerers like me. Huge thanks and thumbs up to Victron! 

As I mentioned before, the Victron's brain of a PV System is made up of the HW (Victron branded or Raspberry PI) and SW - VenusOS. For interacting with the OS UI you have two ways: by web or by touchscreen. Using any ethernet capable device such as Mobile Phone, PC or a Tablet, you can get into VenusOS UI by accessing http://venus.local on your local network. This requires you to connect your device to your LAN. On the other hand, you can interact with the UI without the requirement of a LAN by using a Touchscreen. Victron has its own High Quality-High Priced Touchscreen but ... we want to have this option at the very lowest price ... And this, folks, is where this repo is for. Using a Raspberry Pi with a cheap TFT 3.5" touchscreen display.

## Inspiration

This project has been helped a lot by preceeding people who dealt with this topic before me. I read a lot from forums, webposts and GitHub pages. I want to thank them all.

- https://www.buentutorial.com/touchscreen-3-5-venus-en-raspberry/
- https://github.com/kwindrem
- https://community.victronenergy.com/questions/40195/venus-on-pi-with-35-gpio-screen.html
- https://github.com/victronenergy/venus/wiki/pi-Venus-install-Touch-Screen-%28MPI3501--ILI9486-320x480-LCD%29
- https://gist.github.com/ghettodev/0459eaf9b1ad0754e5dd41ce87cf12fa 
- https://github.com/tasanakorn/rpi-fbcp

## The Cheap LCD Touchscreen for the Raspberry Pi

The Cheap Touchscreen LCDs look like:

![MPI3501-001](https://user-images.githubusercontent.com/35175513/180016009-c5a1cff5-f980-45d6-985b-d9bfeef2145a.jpg)

The one I used is a Kuman 3.5" Inch 480x320 TFT LCD Touch Screen. This comes with a LCD driver ILI9486 and a resistive touch controller XPT2046. There may be variations on the driver and touch controller, but these two are very common. As far as I know, there are other brands out there like the WaveShare one, that are very similar if not identical.

## Hardware requisites

- Tested on a Raspberry Pi 3b+
- Cheap 3.5" TFT LCD Touchscreen (with ILI9486 LCD driver and XPT2046 touchscreen controller)
- microSD card (>= 16GB)
- ethernet cable
- switch/router

Touchscreen mod:

- Soldering iron
- Tin
- Flux (optional)
- Wire cutters

## The issue and the problem

### The issue (backlight control)

Well, the issue is that these cheap touchscreens **do not come with a backlight control** ... So, the screen is on all the time, consuming 75~100mA for the backlight. Altough the ILI9486 has a specific pin for dimming, it is not implemented in these cheap boards.

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

In order to implement hw modification, you will have to find the limiting resistor for the LED backlight string on the PCB. In my case is R5 - 2.2 ohm. So, you need to desolder it and install the next circuit, like shown:

**Bill of Materials**

- R2: choose values between 1k and 1k5 (for sufficient base current) -  1/16 W 
- R3: values between 10k and 56k (for correct base pull-down) -  1/16 W 
- R1: 2R2 - 1/16W (R5 marked in PCB's silkscreen) you have two options:
  * Keep the 0805 SMD resistor and solder one side to the PCB (the pad which is not GND) and solder the other side to R2.
  * Take a THD resistor of the same value and solder it as I did.
I had laying around 2.2 ohm resistors, so it was easy for me. But, I understand that this low value resistors and harder to find, so I would choose to keep SMD resistor and get R2 and R3 that are more common values.

- Q1: 2N3904 or similar NPN with at least 200mA current rating.

**Little remarks**

- Bear in mind that for GND I used pin 8 of U4. It seems picture is not sharp enough on that area.
- For pin 12 of header, you will have to remove the plastic. I used the same soldering iron tip until I got to the pin, applied a bit of flux and then soldered the wire onto it.

![image](https://user-images.githubusercontent.com/35175513/179475164-faaac4ad-1c70-4f27-9ef2-896fc098d743.png)

![image](https://user-images.githubusercontent.com/35175513/179475322-ea8da534-c25f-416f-a57a-051a14aaf7e6.png)

![image](https://user-images.githubusercontent.com/35175513/179475387-592781b8-a9b4-4365-a961-9fae18553d0e.png)

I've chosen RPi GPIO18(PIN12 of header) because it is also capable of PWM and 2N3904 is fine upto 100MHz. So, in case of PWM dimming everything will be fine. But in this solution I have only implemented a ON-OFF solution, for simplicity.

There are similar solutions to this out there but they require you to remove the adhesive of the screen to access the PCB's bottom side, scratch a track and solder a wire on it. This is more difficult and requires you to redo the screen adhesive.

### Software

In terms of sw, I had to write a small C program to manage backlight. The programs controls the GPIO port and automatically shuts the backlight off after any given time value in seconds without touching the display and turns on after touching the screen when backlight is off.

For this, I used WiringPi library and system calls for the touchscreen input device. See ![backlightCtrl.c](https://github.com/wild-mustango/VenusOsTFT35/tree/main/Source/backlightCtrl.c) for more details.

This little program takes two arguments:

- Arg1: input device - Example: /dev/input/touchscreen0
- Arg2: backlight shutdown timer in seconds - Example: 45

Usage example:

```
backlightCtrl /dev/input/touchscreen0 45
```

This command will be lauched by default in a rc.local script I will provide. So, everytime the raspberry boots up, this program will take care of the backlight.

## Duplicating the main fb stream

There is an awesome program for Raspberry Pi called **fbcp** from [tasanakorn](https://github.com/tasanakorn/rpi-fbcp). It's main purpose is "used for copy primary framebuffer to secondary framebuffer" without accepting arguments from command prompt. So, I modified it to accept destination framebuffer via argument. You can find it here ![main.c](https://github.com/wild-mustango/VenusOsTFT35/tree/main/Source/modified_fbcp/main.c)

Usage example:

```
fbcp /dev/fb2
```

### Small compilation note, for those interested in compiling for VenusOS

I tried several times to Cross-Compile from a Linux machine using Victron's SDK, but I was not able to fully setup the compiling environment. I reckon, it is because there are libraries/source code that are private and not available to the public. The way I found to circumvent this, was to compile both programs using another RasPi with Raspbian and then copying the required libraries from the raspbian machine. This is why the following libraries are copied to /usr/lib during the installation process:

backlightCtrl -->
```
libwiringPi.so
libcrypt.so.1
```
fbcp -->
```
libbcm_host.so
libvchiq_arm.so
libvcos.so
```
# Complete Installation instructions

# 1. Install VenusOS into microSD (complete data wipe of previous data) - Tested upto v2.87

## 1.1. Image burning

Go to:

https://updates.victronenergy.com/feeds/venus/release/images/raspberrypi2/

and download latest firmware version. Latest OS version will reside in **venus-image-raspberrypi2.wic.gz** In addition, the OS image will also appear with its full name, for instance (checked on 2022-07-14),  venus-image-raspberrypi2-20220531133203-v2.87.rootfs.wic.gz

.wic.gz are compressed files. I recommend, decompressing the file first. Then, burn the .wic image with [BalenaEtcher](https://www.balena.io/etcher/)

## 1.2. - Superuser, password, ssh and WiFi configuration

For this step, the TFT3.5" touchscreen, is not required to be connected, but if it is, it's OK.

 - After burning VenousOS FW, put the microSD card into the raspberry pi and connect ethernet cable.
 - Power the RaspPi and open http://venus.local in a web browser.
 - At home screen, on the right hand side - HOTKEYS - Click Enter button->Settings->General
 - Click and hold right arrow on the HOTKEYS section until superuser comes up.
 - Set a new superuser password clicking on "Set Root Password"
 - Activate "SSH on LAN". Now you can ssh your RasPi
 - Go to Menu->Settings, scroll down to to "WiFi". Click and a list of available Access Points will appear.
 - Click on yours, insert password and it will Automatically connect.
 - Finally go to Menu->Settings->General and click on reboot twice and unplug ethernet cable.

## 2 TFT3.5" touchscreen software installation and configuration
## 2.1. Installing required files

- Ensure your RasPi is powered off.
- Copy [venus-data.zip](https://github.com/wild-mustango/VenusOsTFT35/blob/main/Installation/venus-data.zip) to a clean fat32 usb drive root folder.
- Plug it into RasPi and power it on.
- Wait until you can connect to venus.local on a web browser.
- go to Settings->VRM online portal -> scroll down to microSD / USB and click on Press to Eject twice.
- Unplug USB drive
- Go back to Settings -> General and Reboot (click twice)
- Wait again until you can connect to venus.local on a web browser
- Finally, go Settings -> General and Reboot (click twice)
- Victron Logo and terminal should come up on-screen.

![on_screen_before_headless off](https://user-images.githubusercontent.com/35175513/179949489-8d742b9e-9b14-4e35-a4ef-fddb1dca7170.jpg)

I have put all the required files inside venus-data.zip. By putting this zipped file inside the USB, when the Raspi boots up, checks USB and decompresses it to /data. One of the decompressed files is rcS.local, in charge of executing a script to put every file in its corresponding folder and inserting configuration into config.txt. As you have seen, you are required to do two reboots. This is intended and required for the correct installation.  The /data is a special folder that survives firmware upgrades done from the GUI.

## 2.2. Config
*****************
Ussing Putty, connect to a venus.local ssh session using root as user and the password setup before.

## 2.2.1 - Installing QT (Venus GUI) mouse driver for the touchscreen and calibration tools for touch


```
opkg update
opkg install qt4-embedded-plugin-mousedriver-tslib
opkg install tslib-calibrate
opkg install tslib-tests
```

### 2.2.2 - Configuring environment variables related to touchscreen. Calibration.
```
reboot
```

After rebooting, it's time to load environment variablos into memory (they will be active during session only)

```
TSLIB_FBDEVICE=/dev/fb0
TSLIB_TSDEVICE=/dev/input/touchscreen0
TSLIB_CALIBFILE=/etc/pointercal
TSLIB_CONFFILE=/etc/ts.conf
TSLIB_PLUGINDIR=/usr/lib/ts
```
And now, calibrate:
```
ts_calibrate
```

![calibration_screen](https://user-images.githubusercontent.com/35175513/179942044-f3cf475e-69fc-4c8f-8138-f272653c9255.jpeg)

NOTE:
By default, start-up rc.local script sets backlight timer to 45s. It may happen that, after reboot, screen has not been touched for 45s. So, you may get a black screen after launching ts_calibrate. Then, your first touch will switch on backlight again but your first calibration point will be wrong. Don't worry, finish this calibration, relaunch ts_calibrate and do your calibration again.

### 2.2.2. - Editing start-up script of VenusOS

To avoid loading environment variables manually after each VenusOS reboot, we have to edit the start-up script of the Venus GUI.

Proceed as follows:
```
nano /opt/victronenergy/gui/start-gui.sh
```
Then add the next lines just below "when headfull" comment block:
```
export TSLIB_TSEVENTTYPE=INPUT
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_TSDEVICE=/dev/input/touchscreen0
export TSLIB_CALIBFILE=/etc/pointercal
export TSLIB_CONFFILE=/etc/ts.conf
export TSLIB_PLUGINDIR=/usr/lib/ts
export QWS_MOUSE_PROTO=tslib:/dev/input/touchscreen0
```
And exit with saving the file using ctr+x,y

After doing this, variables will be automatically loaded after each reboot.

### 2.2.3 Let's tell Venus it "already has a screen"
```
mv /etc/venus/headless /etc/venus/headless.off
reboot
```
FINALLY!! After rebooting you will see Home Page of VenusOS on your cheap TFT3.5" touchscreen.

![venus_os_home_screen](https://user-images.githubusercontent.com/35175513/179940492-51bfa428-b60e-4c6f-af8a-8a708b9890fc.jpeg)


### 2.2.4 Bonus stage: Expanding partitions

This will enlarge your partitions to use as much space available as it can.

```
/opt/victronenergy/swupdate-scripts/resize2fs.sh
reboot
```

Remark: I have noticed that big SD cards (mine is 128GB) only uses up to ~50GB :( don't know the reason.



