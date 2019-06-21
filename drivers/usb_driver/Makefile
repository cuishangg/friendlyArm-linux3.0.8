KERNEL_DRI = /home/cs/aosp/friendlyArm/github-friendlyArm-linux3.0.8

PWD := $(shell pwd)

all:
	make -C $(KERNEL_DRI) M=$(PWD) modules

clean:
	make -C $(KERNEL_DRI) M=$(PWD) modules clean
	rm -rf modules.order

obj-m += usbmouse_as_key.o

