# use pwm to generate triggering clock

# 30FPS: period=1/fps (nanoseconds) and duty_cycle 50% (nanoseconds)
# NOTE: timing MUST BE in nanoseconds!
echo 333333333 > /sys/class/pwm/pwmchip1/pwm0/period
echo 150000000 > /sys/class/pwm/pwmchip1/pwm0/duty_cycle

# enable clock
echo 1 > /sys/class/pwm/pwmchip1/pwm0/enable
