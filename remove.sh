#!/bin/sh

module="chardev"
device="chardev"

bash filter.sh

sudo /sbin/rmmod $module || exit 1

echo Das Chardev Modul wurde aus dem Kernel entfernt.

bash filter.sh

sudo rm /dev/$device
