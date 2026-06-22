#!/bin/bash

#cat /sys/class/power_supply/BAT1/charge_full | tr '\n' '\0' > /dev/serial/by-id/usb-STM*
#cat /sys/class/power_supply/BAT1/charge_now | tr '\n' '\0' > /dev/serial/by-id/usb-STM*
while true
do
	echo "data sent!"
	echo " "
	FULL=$(cat /sys/class/power_supply/BAT1/charge_full | tr '\n' ''\0'' )
	NOW=$(cat /sys/class/power_supply/BAT1/charge_now | tr '\n' ''\0'' )
	echo "	[full]:$FULL"
	echo "	[current]:$NOW"
	python3 -c "print(int(($NOW/$FULL)*100))" | tr -d '\n' > /dev/serial/by-id/usb-STM*
	echo " "
	sleep 5
done
