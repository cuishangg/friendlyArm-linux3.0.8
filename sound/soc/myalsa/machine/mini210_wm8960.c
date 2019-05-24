/*
 *myalsa driver ALSA ASOC machine file.
 *made by cuishang.
 */

#include <linux/clk.h>
#include <linux/gpio.h>

#include <sound/soc.h>


static struct platform_device mini210_wm8960_snd_device;

static struct snd_soc_ops *mini210_wm8960_ops = {


};

static struct snd_soc_dai_link *snd_soc_mini210_wm8960_link = {
	.stream_name = "cuishang",		/* Stream name */
	.name = "cuishang",			/* Codec name */
	.codec_name = "cwm8960",		/* for multi-codec */
	.platform_name = "mini210-iic",	/* for multi-platform */
	.cpu_dai_name = "mini210-iis",
	.codec_dai_name = "cwm8960-iis",
    .ops = &mini210_wm8960_ops,
};


static struct snd_soc_card snd_soc_mini210_wm8960 = {
    .name = "myalsa",
    .dai_link = &snd_soc_mini210_wm8960_link,
	.num_links = 1,
};


static int mini210_wm8960_probe(struct platform_device *pdev)
{
    mini210_wm8960_snd_device = platform_device_alloc("soc-audio", -1);
    platform_set_drvdata(mini210_wm8960_snd_device, &snd_soc_mini210_wm8960);
    ret = platform_device_add(mini210_wm8960_snd_device);
}

static int mini210_wm8960_remove(struct platform_device *pdev)
{
    platform_device_unregister(&mini210_wm8960_snd_device);
}


static struct platform_driver mini210_wm8960_driver = {
    .probe  = mini210_wm8960_probe,
    .remove = mini210_wm8960_remove,
    .driver = {
        .name = "mini210_wm8960",
        .owner = THIS_MODULE,
    },
};

static void mini210_wm8960_device_release(struct device *dev)
{

}

static struct platform_device mini210_wm8960_device = {
    .name         = "mini210_wm8960 ",
    .id       = -1,
    .dev = { 
    	.release = mini210_wm8960_device_release, 
	},
};


static int __init mini210_wm8960_init(void)
{
    platform_driver_register(&mini210_wm8960_driver);
    platform_device_register(&mini210_wm8960_device);
    
    return 0;
}

static void __exit mini210_wm8960_exit(void)
{
    platform_driver_unregister(&mini210_wm8960_driver);
    
    platform_device_unregister(&mini210_wm8960_device);
}

module_init(mini210_wm8960_init);
module_exit(mini210_wm8960_exit);

