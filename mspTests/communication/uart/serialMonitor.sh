#!/bin/bash
# serial monitor, please chage the device parameter for yours device file
# my default device file is /dev/ttyACM0
# press ctrl+a ctrl+k and y to kill the serial monitor ctrl+c doesn't work

device=/dev/ttyACM0
sudo stty -F $device cs8 9600
sudo screen $device 9600
