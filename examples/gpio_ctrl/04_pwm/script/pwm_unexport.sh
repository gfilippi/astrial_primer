
# disable clock
echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable

# unexport pwm
echo 0 > /sys/class/pwm/pwmchip0/unexport


