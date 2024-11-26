# use pwm to generate triggering clock

# 30FPS: period=1/fps (nanoseconds) and duty_cycle 50% (nanoseconds)
echo 33333333 > /sys/class/pwm/pwmchip0/pwm0/period
echo 15000000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle

# enable clock
echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
