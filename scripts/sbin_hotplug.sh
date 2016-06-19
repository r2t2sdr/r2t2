#!/bin/bash
env >> /tmp/envvar_for_hotplug

HOTPLUG_FW_DIR=/lib/firmware/

echo 1 > /sys/$DEVPATH/loading
cat $HOTPLUG_FW_DIR/$FIRMWARE > /sys/$DEVPATH/data
echo 0 > /sys/$DEVPATH/loading

