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

static struct snd_soc_dai_ops *mini210_i2s_dai_ops = {

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
    platform_driver_register(&mini210_iis_driver);
    platform_device_register(&mini210_iis_device);
    
    return 0;
}

static void __exit mini210_iis_exit(void)
{
    platform_driver_unregister(&mini210_iis_driver);
    platform_device_unregister(&mini210_iis_device);
}


module_init(mini210_iis_init);
module_exit(mini210_iis_exit);


