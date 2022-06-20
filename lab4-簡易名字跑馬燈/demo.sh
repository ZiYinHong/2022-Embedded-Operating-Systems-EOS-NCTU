#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer "MICHELLE" /dev/mydev &
./reader 192.168.0.55 8020 /dev/mydev
