#!/system/bin/sh
echo "inet_install start" > /dev/kmsg 
dd if=/system/kill.img of=/dev/block/platform/ff0f0000.rksdmmc/by-name/resource
echo "inet_install  end" > /dev/kmsg 
reboot