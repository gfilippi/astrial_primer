#!/bin/bash
#
# Jeff Geerling's super-rudimentary fan controller script for the CM4.
#
# Use:
#   1. Download this script to a path like `/opt/cm4-fan.sh`
#   2. Run it with: `nohup bash /opt/cm4-fan.sh`
#   3. Profit!
#
# You should wrap it in a systemd unit file if you want it to persist and come
# up after reboot, too.

# Explicitly set $PATH so i2c tools are found.
PATH=$PATH:/usr/sbin

# Temperature set point (Celcius).
TEMP_THRESHOLD=41
TEMP_COMPARE=$(($TEMP_THRESHOLD * 1000))

TEMP_MAX=55
T_MAX=$(($TEMP_MAX * 1000))

TEMP_MIN=30
T_MIN=$(($TEMP_MIN * 1000))

# Minimum fan speed (0-255).
MIN_FAN_SPEED=30
MIN_FAN_SPEED_HEX=$(printf "0x%02x" "$MIN_FAN_SPEED")

# Maximum fan speed (0-255).
MAX_FAN_SPEED=255
MAX_FAN_SPEED_HEX=$(printf "0x%02x" "$MAX_FAN_SPEED")

# TODO: The script should also check that `i2cget` is present, and that the
# fan is visible on the bus.

# Start a loop.
while true; do
    # Get the current temperature.
    CURRENT_TEMP=$(cat /sys/devices/virtual/thermal/thermal_zone0/temp)

    #echo $CURRENT_TEMP

    P_alfa=$(( (($MAX_FAN_SPEED - $MIN_FAN_SPEED)*10000) / ($T_MAX - $T_MIN) ))

    CURRENT_FAN_SPEED=$(( $MIN_FAN_SPEED + $P_alfa * ($CURRENT_TEMP - $T_MIN) / 10000  ))

    if [ "$CURRENT_FAN_SPEED" -ge "255" ]; then
       CURRENT_FAN_SPEED=255
    fi

    CURRENT_FAN_SPEED_HEX=$(printf "0x%02x" "$CURRENT_FAN_SPEED")

    #echo $CURRENT_FAN_SPEED

    # PID: only Proportional, set fan speed
    i2cset -y 1 0x2f 0x30 $CURRENT_FAN_SPEED_HEX


#    if [ $CURRENT_TEMP -ge $TEMP_COMPARE ]; then
#        # If current temperature is more than desired, set fan to maximum speed.
#        i2cset -y 1 0x2f 0x30 $MAX_FAN_SPEED_HEX
#    else
#        # If current temperature is good, set fan to minimum speed.
#        i2cset -y 1 0x2f 0x30 $MIN_FAN_SPEED_HEX
#    fi

    sleep 3
done
