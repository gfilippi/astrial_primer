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

GPIO_LINE = 1
GPIO_DEVICE = "/dev/gpiochip1"

# Ensure the GPIO device is valid
assert gpiod.is_gpiochip_device(GPIO_DEVICE)

# Request control over the GPIO lines with initial settings
with gpiod.request_lines(
    GPIO_DEVICE,
    consumer="blink-example",
    config={
        # Configure the LED line as output and initially inactive
        GPIO_LINE: gpiod.LineSettings(
            direction=Direction.OUTPUT, output_value=Value.INACTIVE
        ),
    },
) as request:
    # Loop indefinitely to blink the RGB LED
    while True:
        request.set_value(GPIO_LINE, Value.ACTIVE)
        time.sleep(0.2)
        request.set_value(GPIO_LINE, Value.INACTIVE)
        time.sleep(0.2)

