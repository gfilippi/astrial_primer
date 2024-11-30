
# disable clock
echo 0 > /sys/class/pwm/pwmchip1/pwm0/enable

# unexport pwm
echo 0 > /sys/class/pwm/pwmchip1/unexport


