snd-soc-mini210-wm8960-objs := mini210_wm8960.o
obj-m += snd-soc-mini210-wm8960.o

snd-soc-wm8960-objs := wm8960.o
obj-m += snd-soc-wm8960.o

KERNELDIR ?= /home/cuishang/as/work/friendlyArm-linux3.0.8
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install: default

	INSTALL_MOD_PATH=../../ $(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
