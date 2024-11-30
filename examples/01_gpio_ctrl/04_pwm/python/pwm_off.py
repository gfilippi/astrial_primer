# see: https://github.com/scottellis/pwmpy for details

from pwm import PWM

# select PWM hw and export pin
pwm0=PWM(chip=1, channel=0)

# export (redundant, just in case it was not exported already)
pwm0.export()

pwm0.enable = False

pwm0.unexport()

