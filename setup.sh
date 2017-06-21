#!/bin/bash

module="chardev"
device="chardev"

# all users can read and write but cannot execute
mode=666

#remove device-node
sudo rm -f /dev/$device

#applying the module to the kernel
sudo /sbin/insmod ./$module.ko $* || exit 1

bash filter.sh

#sudo tail -f /var/log/kern.log

#getting the dynamic major number of the device
major=$(grep $module /proc/devices | cut -c-4)

#creating a device-node
sudo mknod /dev/$device c $major 0

#change file permissions
sudo chmod $mode /dev/$device

#echo Ein device fuer major nummer $major wurde erstellt
#mknod /dev/${device}f c $major 
