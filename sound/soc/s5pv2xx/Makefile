# Samsung Platform Support
MAKE_MODE = yes

ifeq ($(MAKE_MODE),yes)
KERN_DIR = /home/cuishang/as/work/friendlyArm-linux3.0.8

all:
	make -C $(KERN_DIR) M=`pwd` modules 

    obj-m += dma.o
    obj-m += s5pc1xx-i2s.o
    obj-m += mini210_wm8960.o
    obj-m += wm8960.o

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

else
    #snd-soc-s5pv2xx-objs := dma.o
    #snd-soc-s5pc1xx-i2s-objs := s5pc1xx-i2s.o
    #snd-soc-mini210-wm8960-objs := mini210_wm8960.o
    #snd-soc-wm8960-objs := wm8960.o

    obj-$(CONFIG_SND_S5PV2XX_SOC) += dma.o
    obj-$(CONFIG_SND_S5PC1XX_I2S) += s5pc1xx-i2s.o
    obj-$(CONFIG_SND_MINI210_WM8960) += mini210_wm8960.o
    obj-$(CONFIG_SND_WM8960) += wm8960.o

endif


