
# WARNING: GPIOSYSFS is DEPRECATED, use LIBGPIOD
#
# PIN=15
#
# echo $PIN > /sys/class/gpio/export
# echo out > /sys/class/gpio/gpio${PIN}/direction
# echo 1 > /sys/class/gpio/gpio${PIN}/value;
# usleep 10000;
# echo 0 > /sys/class/gpio/gpio${PIN}/value

# to show gpio status:
# gpiodetect

# to list all gpio configured
# gpioinfo


# PIN15 is on BANK #0
gpioset gpiochip0 15=1
usleep 1000
gpioset gpiochip0 15=0

