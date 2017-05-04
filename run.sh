#!/bin/bash
echo 44 > /sys/class/gpio/unexport
echo 45 > /sys/class/gpio/unexport
echo 68 > /sys/class/gpio/unexport
echo 69 > /sys/class/gpio/unexport

echo 44 > /sys/class/gpio/export
echo 45 > /sys/class/gpio/export
echo 68 > /sys/class/gpio/export
echo 69 > /sys/class/gpio/export
/home/debian/projects/almas/runme
