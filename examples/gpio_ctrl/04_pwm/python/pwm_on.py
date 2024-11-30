# see: https://github.com/scottellis/pwmpy for details

from pwm import PWM

# select PWM hw and export pin
pwm0=PWM(chip=1, channel=0)

# export (once!) before use
pwm0.export()

# values in nanoseconds

# Setup a 30FPS period, 50% duty cycle pulse (period and duty_cycle units are nanoseconds)
pwm0.period = 333333333
pwm0.duty_cycle = 150000000

# turn on timer
pwm0.enable = True


