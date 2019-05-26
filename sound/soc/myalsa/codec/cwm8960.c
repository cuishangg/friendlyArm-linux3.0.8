/*
 *myalsa driver ALSA ASOC codec file.
 *made by cuishang.
 */
    
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <sound/wm8960.h>


#define SNDRV_PCM_RATE_8000_48000	(SNDRV_PCM_RATE_8000_44100|SNDRV_PCM_RATE_48000)

#define WM8960_RATES SNDRV_PCM_RATE_8000_48000

#define WM8960_FORMATS \
	(SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | \
	SNDRV_PCM_FMTBIT_S24_LE)


static struct snd_soc_codec_driver *soc_codec_dev_cwm8960 = {



};

static int cwm8960_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
    /*根据params的值，设置wm8960的寄存器*/
    return 0;
}


static struct snd_soc_dai_ops *cwm8960_dai_ops = {
	.hw_params = cwm8960_hw_params,
};

static struct snd_soc_dai_driver *cwm8960_dai = {
	.name = "cwm8960-iis",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8960_RATES,
		.formats = WM8960_FORMATS,},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8960_RATES,
		.formats = WM8960_FORMATS,},
	.ops = &cwm8960_dai_ops,
};


static int cwm8960_i2c_driver_probe(struct i2c_client *i2c_client, const struct i2c_device_id *i2c_device_id)
{
	return snd_soc_register_codec(&i2c->dev, &soc_codec_dev_cwm8960, &cwm8960_dai, 1);
}

static int cwm8960_i2c_driver_remove(struct i2c_client *i2c_client)
{
    snd_soc_unregister_codec(&i2c->dev);
    return 0;
}

static struct i2c_device_id *cwm8960_i2c_id[] = {
	{ "cwm8960-iic", 0 },
	{ }
};

static struct i2c_driver *cwm8960_i2c_driver = {
    .probe = cwm8960_i2c_driver_probe,
    .remove = cwm8960_i2c_driver_remove,
    .driver = {
        .name = "cwm8960-codec",
        .owner = THIS_MODULE,
    }
	.id_table = cwm8960_i2c_id,
};

static int __init cwm8960_mod_init(void)
{
    return i2c_add_driver(&cwm8960_i2c_driver);
}

static void __init cwm8960_mod_exit(void)
{
    i2c_del_driver(&cwm8960_i2c_driver);
}


module_init(cwm8960_mod_init);
module_exit(cwm8960_mod_exit);

