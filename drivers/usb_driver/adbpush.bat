adb push ./usbmouse_as_key.ko /data/
adb shell "sync"
adb shell "rmmod usbmouse_as_key"
adb shell "insmod /data/usbmouse_as_key.ko"
pause