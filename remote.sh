#!/bin/csh

setenv PATH /home/engtien/Trek2000/arm-2010q1/bin:$PATH
make distclean ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-
make ka2000_config ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-
make all ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-
#make u-boot.dis ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-

arm-none-linux-gnueabi-objdump -d u-boot > u-boot.dis

if (! $?ARMLMD_LICENSE_FILE) then
        setenv ARMLMD_LICENSE_FILE 8224@klws01
else
        setenv ARMLMD_LICENSE_FILE 8224@klws01:${ARMLMD_LICENSE_FILE}
endif
source /edatools/ARM/rvds30/RVDS30env.csh
fromelf u-boot -c --output u-boot.vhx --8x1 --vhx

#../TOOLS/bin2hex u-boot.bin u-boot.hex
#../TOOLS/vhx2mif u-boot.vhx u-boot.mif
