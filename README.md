U-Boot port for Transcend WiFi SD cards 
=======================================

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

WARNING: alpha quality software, read the instructions in the installation section.

This project fixes the U-Boot port published by Transcend in source format. The SD controller driver was broken, not allowing access from U-Boot and leaving the controller in an inconsistent state which prevented the Linux driver from working as well.

A few other improvements have been implemented. Look below for a complete list.

Note that this port is [hacked in and held together by duct tape](http://cdn.edu-search.com/uploads/duct-tape.jpg), but since the required functionality is quite limited, I don't think it would be worth the time to clean it up and merge it with a more recent U-Boot version.


Build instructions
------------------

You'll need to have an ARM toolchain set up. If you're cross-compiling on x86, you can use [the one built by Linaro](http://www.linaro.org/downloads/), [Sourcery CodeBench](http://www.mentor.com/embedded-software/sourcery-tools/sourcery-codebench/editions/lite-edition/), one provided by your distribution, etc.

If you're cross-compiling, you'll need to append the following string *at the end* of each command:

    ARCH=arm CROSS_COMPILE=prefix-of-toolchain
    
Where prefix-of-toolchain will probably looks something like *arm-none-eabi-*.

Clean:

    make distclean

Config:

    make ka2000_config
    
Build:

    make


Installation
------------

WARNING: since this bootloader hasn't been tested thoroughly it is highly recommended that you set up an UART terminal for debugging. See [this thread](https://forum.openwrt.org/viewtopic.php?id=45820) for the pinout.

* As a second stage bootloader

Because there's a possibility of bricking the ARM SoC part of the card each time the bootloader is flashed, it is highly recommended to simply use it as a second stage bootloader loaded from the SD card by the original U-Boot binary.

To do this, rename the *u-boot.bin* file produced by the build process to *program.bin* and copy it to the root of the FAT partition on your card.

* Flashing the device

WARNING: If you choose to install this version in the flash memory, be warned: automatic loading of program.bin has been disabled, which means that you will *need* an UART terminal to upgrade any of the bootloader, kernel, or rootfs images.

To flash u-boot.bin, copy it together with program.bin from one of the binary firmware upgrade packages to the root of the FAT partition on the card.


Changes from the official version
---------------------------------

* Fixed the SD controller driver, which works in the binary release but not in the source release.
* Disabled automatic loading of program.bin, to prevent accidental flashing and to allow loading from the SD card of this version as a second stage bootloader (see installation as a second stage bootloader above) without entering a loop.
* Fix access to the SD from U-Boot when the device is simply powered on but not connected to an SD card reader. This will enable stand-alone use. Note that even though U-Boot now works in this case, the Transcend-provided Linux system doesn't. I'm working on this.

Known issues
------------

* While this version of U-Boot can run in stand-alone mode (with power applied but without the device being connected to an SD card reader), the Transcend-provided Linux system still doesn't work in that mode. I'm working on a fix.
* If an external card reader writes to the SD while u-boot is running, all subsequent SD operations from u-boot will fail. A power cycle is required.

