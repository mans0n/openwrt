#!/bin/sh

CPU_TEMP="$(cut -c1-2 /sys/class/thermal/thermal_zone0/temp)"

CPU_LOW="$(cut -c1-2 /sys/class/thermal/thermal_zone0/trip_point_1_temp)"
CPU_HIGH="$(cut -c1-2 /sys/class/thermal/thermal_zone0/trip_point_2_temp)"

if [ -d /sys/devices/platform/gpio-fan ]; then
	FAN_CTRL=/sys/devices/platform/gpio-fan/hwmon/hwmon0/pwm1
else
	exit 0
fi

if [ "$CPU_TEMP" -ge "$CPU_HIGH" ]; then
	echo "255" > "$FAN_CTRL"
elif [ "$CPU_TEMP" -ge "$CPU_LOW" ]; then
	echo "100" > "$FAN_CTRL"
else
	echo "0" > "$FAN_CTRL"
fi
