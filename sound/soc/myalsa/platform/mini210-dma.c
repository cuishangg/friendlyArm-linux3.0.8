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

#define PCM_INFO	\
	( SNDRV_PCM_INFO_INTERLEAVED |	\
	  SNDRV_PCM_INFO_BLOCK_TRANSFER |	\
	  SNDRV_PCM_INFO_MMAP  | SNDRV_PCM_INFO_MMAP_VALID |	\
	  SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME )
#define PCM_FORMAT	\
	( SNDRV_PCM_FMTBIT_S16_LE |	\
	  SNDRV_PCM_FMTBIT_U16_LE |	\
	  SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S8 )

static const struct snd_pcm_hardware dma_hardware = {
	.info				= PCM_INFO,
	.formats			= PCM_FORMAT,
	.channels_min		= 2,
	.channels_max		= 2,
	.buffer_bytes_max	= 128 * 1024,
	.period_bytes_min	= 128,
	.period_bytes_max	= 32 * 1024,
	.periods_min		= 2,
	.periods_max		= 128,
	.fifo_size			= 32,
};


static int mini210_dma_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	pr_debug("Entered %s\n", __func__);

	snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);
	snd_soc_set_runtime_hwparams(substream, &dma_hardware);

	return 0;
}

static int mini210_dma_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
    /*根据params的值设置dma*/
    return 0;
}


struct snd_pcm_ops *mini210_dma_ops = {
	.open		= mini210_dma_open,
    .hw_params	= mini210_dma_hw_params,
};

static struct snd_soc_platform_driver *mini210_dma_platform = {
	.ops		= &mini210_dma_ops,
	//.pcm_new	= dma_new,
	//.pcm_free	= dma_free_dma_buffers,
};

static int mini210_dma_probe(struct platform_device *pdev)
{
    return snd_soc_register_platform(&pdev->dev, &mini210_dma_platform);
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

