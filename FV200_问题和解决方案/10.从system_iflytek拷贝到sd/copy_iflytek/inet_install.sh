#!/system/bin/sh
echo "inet_install in" > /dev/kmsg
if [ ! -e /data/system.notfirstrun ]; then
echo "firstrun of system" > /dev/kmsg
#set defaulttime   
# if [ ! "`getprop ro.inet.defaulttime`" = "" ];then
#	/system/bin/date -s  `getprop ro.inet.defaulttime`
#  fi
  if [ -d /system/iflytek ]; then
  	echo "copy  iflytek" > /dev/kmsg
	/system/bin/cp -R /system/iflytek /sdcard/
  fi

  

/system/bin/touch /data/system.notfirstrun

fi
echo "inet_install exit" > /dev/kmsg

