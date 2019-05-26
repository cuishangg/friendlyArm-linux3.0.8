 /*
  *myalsa driver ALSA ASOC CPU-DAI file.
  *made by cuishang.
  */

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/regulator/consumer.h>
#include <linux/wakelock.h>

#include <sound/soc.h>
#include <sound/pcm_params.h>

#include <plat/regs-iis.h>
#include <plat/audio.h>

#include <mach/dma.h>
#include <mach/map.h>
#include <mach/regs-audss.h>
#include <mach/regs-clock.h>


#define MINI210_I2S_RATES \
	(SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 | SNDRV_PCM_RATE_16000 | \
	 SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 | \
	 SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000 | \
	 SNDRV_PCM_RATE_KNOT)

#define MINI210_I2S_FMTS \
	(SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_S16_LE |\
	 SNDRV_PCM_FMTBIT_S24_LE)

static struct mini210_iis_regs {
    unsigned int iiscon ; 
    unsigned int iismod ;
    unsigned int iisfic ;
    unsigned int iispsr ; 
    unsigned int iistxd ;  
    unsigned int iisrxd ; 
    unsigned int iisfics ;

};

static volatile unsigned int *gpicon;
static volatile struct mini210_iis_regs *iis_regs;

static int mini210_iis_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params,
		struct snd_soc_dai *dai)
{
    /*params的值设置mini210 iis的寄存器*/

    int N;
    // 配置引脚用于i2s功能
    gpicon = 0x22222222;

    // 设置i2s控制器
    // step 4:	Divider of IIS (67.7 -> 11.289Mhz)
    // N + 1 = (67.7Mhz) / (256 * 44.1Khz) = 5.99
    // IISCDCLK  11.289Mhz = 44.1K * 256fs
    // IISSCLK	  1.4112Mhz = 44.1K * 32fs
    // IISLRCLK   44.1Khz
    N = 5;
    iis_regs->iispsr = 1<<15 | N<<8;

    // IIS interface active (start operation).	1 = Active
    iis_regs->iiscon |= 1<<0 | (unsigned)1<<31;

    // [9:8] 10 = Transmit and receive simultaneous mode
    // 1 = Using I2SCLK 	(use EPLL)
    iis_regs->iismod = 1<<9 | 0<<8 | 1<<10;

    return 0;
}


static struct snd_soc_dai_ops *mini210_i2s_dai_ops = {
    .hw_params = mini210_iis_hw_params,
};

static struct snd_soc_dai_driver *mini210_i2s_dai = {
    .name = "cuishang-iis",
    .playback = {
        .channels_min = 2,
        .channels_max = 2,
        .rates = MINI210_I2S_RATES,
        .formats = MINI210_I2S_FMTS,
    },
    .ops = &mini210_i2s_dai_ops,
};


static int mini210_iis_probe(struct platform_device *pdev)
{
    
    return snd_soc_register_dai(&pdev->dev, &mini210_i2s_dai);
}

static int mini210_iis_remove(struct platform_device *pdev)
{
    return snd_soc_unregister_dai(&pdev->dev);
}

static struct platform_driver mini210_iis_driver = {
    .probe  = mini210_iis_probe,
    .remove = mini210_iis_remove,
    .driver = {
        .name = "mini210-iis",
        .owner = THIS_MODULE,
    },
};

static void mini210_iis_device_release(struct device *dev)
{

}

static struct platform_device mini210_iis_device = {
    .name         = "mini210-iis",
    .id       = -1,
    .dev = { 
    	.release = mini210_iis_device_release, 
	},
};



static int __init mini210_iis_init(void)
{
    gpicon = ioremap(0xE0200220, 4);
    iis_regs = ioremap(0xEEE30000, sizeof(struct mini210_iis_regs));
    platform_driver_register(&mini210_iis_driver);
    platform_device_register(&mini210_iis_device);
    
    return 0;
}

static void __exit mini210_iis_exit(void)
{
    platform_driver_unregister(&mini210_iis_driver);
    platform_device_unregister(&mini210_iis_device);
    iounmap(gpicon);
    iounmap(iis_regs);
}


module_init(mini210_iis_init);
module_exit(mini210_iis_exit);


