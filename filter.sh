#!/bin/bash
#print the content of the chardev module

modul=chardev

sudo lsmod | grep $modul
