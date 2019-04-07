#!/bin/bash

# (OSX only) Creates an sdcard image, the sdcard should have a raspian ditro installed
# Remember to install support for ext4 FS:
#   * brew cask install osxfuse
#   * brew install ext4fuse
#
# Source: https://raspberrypi.stackexchange.com/questions/13437/how-to-mount-a-raspbian-sd-card-on-a-mac

export DSK=`diskutil list | grep "Linux" | cut -c 69-73`
if [ $DSK ]; then
    echo $DSK
else
    echo "Disk not found"
    exit
fi
diskutil unmountDisk /dev/$DSK
echo Please wait, it could take a while...
sudo dd of=./sdcardBackups/Piback.img if=/dev/$DSK bs=2m
echo Backup completed - now compressing
gzip -9 ./sdcardBackups/Piback.img
#rename to current date
mv ./sdcardBackups/Piback.img.gz "./sdcardBackups/Piback`date +%Y%m%d`.img.gz"
