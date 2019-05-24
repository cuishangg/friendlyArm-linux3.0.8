/*
 *myalsa driver ALSA ASOC CPU-DMA file.
 *made by cuishang.
 */
#include <sound/soc.h>

#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <plat/regs-iis.h>


static struct snd_soc_platform_driver mini210_dma_dai = {
};

static int mini210_dma_probe(struct platform_device *pdev)
{
    return snd_soc_register_platform(&pdev->dev, &mini210_dma_dai);
}

static int mini210_dma_remove(struct platform_device *pdev)
{
    return snd_soc_unregister_platform(&pdev->dev);
}

static struct platform_driver mini210_dma_driver = {
    .probe  = mini210_dma_probe,
    .remove = mini210_dma_remove,
    .driver = {
        .name = "mini210-dma",
        .owner = THIS_MODULE,
    },
};

static void mini210_dma_device_release(struct device *dev)
{

}

static struct platform_device mini210_dma_device = {
    .name         = "mini210-dma",
    .id       = -1,
    .dev = { 
    	.release = mini210_dma_device_release, 
	},
};

static int __init mini210_dma_init(void)
{
    platform_driver_register(&mini210_dma_driver);
    platform_device_register(&mini210_dma_device);
    
    return 0;
}

static void __exit mini210_dma_exit(void)
{
    platform_driver_unregister(&mini210_dma_driver);
    platform_device_unregister(&mini210_dma_device);
}


module_init(mini210_dma_init);
module_exit(mini210_dma_exit);

