#
# GPIO trigger using python an libgpiod
#
# see also: https://github.com/brgl/libgpiod/tree/master/bindings/python/examples

import time
from gpiod.line import Direction, Value
import gpiod


# see ASTRIAL pin map for gpio selection
# GPIO(N) gpiochip(N)    lane
# ===========================
# GPIO23  gpiochip1       1
# GPIO24  gpiochip1       2
# GPIO25  gpiochip1       3

GPIO_LINE_B = 1
GPIO_LINE_G = 2
GPIO_LINE_R = 3
GPIO_DEVICE = "/dev/gpiochip1"

# Ensure the GPIO device is valid
assert gpiod.is_gpiochip_device(GPIO_DEVICE)

# Request control over the GPIO lines with initial settings
with gpiod.request_lines(
    GPIO_DEVICE,
    consumer="blink-example",
    config={
        GPIO_LINE_B: gpiod.LineSettings(
            direction=Direction.OUTPUT, output_value=Value.INACTIVE
        ),
        GPIO_LINE_G: gpiod.LineSettings(
            direction=Direction.OUTPUT, output_value=Value.INACTIVE
        ),
        GPIO_LINE_R: gpiod.LineSettings(
            direction=Direction.OUTPUT, output_value=Value.INACTIVE
        )
    },
) as request:
   request.set_value(GPIO_LINE_R, Value.INACTIVE)
   request.set_value(GPIO_LINE_G, Value.INACTIVE)
   request.set_value(GPIO_LINE_B, Value.INACTIVE)
