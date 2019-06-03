/*
 * mini_wm8960.c  --  WM8960 ALSA SoC Audio driver
 *
 * Author: happyzlz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <plat/regs-iis.h>
#include <sound/soc.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include "s5pc1xx-i2s.h"
#include "wm8960.h"

#define DEBUG

#ifdef	DEBUG
#define	dprintk( argc, argv... )		printk( argc, ##argv )
#else
#define	dprintk( argc, argv... )		
#endif

static struct snd_soc_card mini210_soc_card;

static int mini210_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	unsigned int rate = params_rate(params);
	snd_pcm_format_t fmt = params_format( params );
	int	ret = 0;

	int bclk_div;
	int rclk_div;
	unsigned int prescaler_div;
	
	int sl;	
	unsigned int mul;

	dprintk("+%s()\n", __FUNCTION__ );

	switch ( fmt ) {
		case SNDRV_PCM_FORMAT_S16:
			bclk_div = 32;
			break;
		case SNDRV_PCM_FORMAT_S20_3LE:
		case SNDRV_PCM_FORMAT_S24:
			bclk_div = 48;
			break;
		default:
			dprintk("-%s(): PCM FMT ERROR\n", __FUNCTION__ );
			return -EINVAL;
	}
	
	switch ( rate ) {
		case 8000:
		case 11025:
		case 12000:
			if( bclk_div == 48 )
				rclk_div = 768;	//0x300
			else
				rclk_div = 512;	//0x200
			break;

		case 16000: 
		case 22050: 
		case 24000:
		case 32000:
		case 44100:	//AC44
		case 48000: 
		case 88200:
		case 96000:
			if( bclk_div == 48 )
				rclk_div = 384;	//0x180
			else
				rclk_div = 256;	//0x100
			break;

		case 64000:
			rclk_div = 384;		//0x180
			break;

		default:
			dprintk("-%s(): SND RATE ERROR\n", __FUNCTION__ );
			return -EINVAL;
	}
	
	ret = snd_soc_dai_set_fmt( codec_dai, SND_SOC_DAIFMT_I2S|SND_SOC_DAIFMT_NB_NF|SND_SOC_DAIFMT_CBS_CFS );
	if( ret < 0 ){
		dprintk( "-%s(): Codec DAI configuration error, %d\n", __FUNCTION__, ret );
		return ret;
	}

	ret = snd_soc_dai_set_fmt( cpu_dai, SND_SOC_DAIFMT_I2S|SND_SOC_DAIFMT_NB_NF|SND_SOC_DAIFMT_CBS_CFS );
	if( ret < 0 ){
		dprintk( "-%s(): AP DAI configuration error, %d\n", __FUNCTION__, ret );
		return ret;
	}

	ret = snd_soc_dai_set_sysclk( cpu_dai, S3C64XX_CLKSRC_CDCLK, rate, SND_SOC_CLOCK_OUT );	//44100
	if( ret < 0 ){
		dprintk( "-%s(): AP sycclk CDCLK setting error, %d\n", __FUNCTION__, ret );
		return ret;
	}

	ret = snd_soc_dai_set_sysclk( cpu_dai, S3C_CLKSRC_CLKAUDIO, rate, SND_SOC_CLOCK_OUT );	//44100
	if( ret < 0 ){
		dprintk( "-%s(): AP sysclk CLKAUDIO setting error, %d\n", __FUNCTION__, ret );
		return ret;
	}

	if( rate & 0x0F )
		sl = 0x04099990;
	else
		sl = 0x02ee0000;

	ret = snd_soc_dai_set_clkdiv( codec_dai, WM8960_SYSCLKDIV, WM8960_SYSCLK_DIV_1 );
	if( ret < 0 ){
		dprintk( "-%s(): Codec SYSCLKDIV setting error, %d\n", __FUNCTION__, ret );
		return ret;
	}

	ret = snd_soc_dai_set_clkdiv( codec_dai, WM8960_DACDIV, WM8960_DAC_DIV_1  );
	if( ret < 0 ){
		dprintk( "-%s(): Codec DACDIV setting error, %d\n", __FUNCTION__, ret );
		return ret;
	}

	mul = rate * rclk_div;
	ret = snd_soc_dai_set_clkdiv( codec_dai, WM8960_DCLKDIV, mul );
	if( ret < 0 ){
		dprintk( "-%s(): Codec DCLKDIV setting error, %d\n", __FUNCTION__, ret );
		return ret;
	}
	
	prescaler_div =  sl / mul;
	prescaler_div--;
	ret = snd_soc_dai_set_clkdiv( cpu_dai, S3C64XX_DIV_PRESCALER, prescaler_div );
	if( ret < 0 ){
		dprintk( "-%s(): AP prescalar setting error, %d\n", __FUNCTION__, ret );
		return ret;
	}

	ret = snd_soc_dai_set_clkdiv( cpu_dai, S3C64XX_DIV_RCLK, rclk_div );
	if( ret < 0 ){
		dprintk( "-%s(): AP RFS setting error, %d\n", __FUNCTION__, ret );
		return ret;
	}

	ret = snd_soc_dai_set_clkdiv( cpu_dai, S3C64XX_DIV_BCLK, bclk_div );
	if( ret < 0 ){
		dprintk( "-%s(): AP BFS setting error, %d\n", __FUNCTION__, ret );
		return ret;
	}
	
	dprintk("-%s()\n", __FUNCTION__ );
	return 0;
}

static struct snd_soc_ops mini210_wm8960_ops = {
	.hw_params = mini210_hw_params,
};

static const struct snd_soc_dapm_widget mini210_dapm_capture_widgets[] = {
	SND_SOC_DAPM_MIC(	"Mic Jack",			NULL ),
	SND_SOC_DAPM_LINE(	"Line Input 3 (FM)",NULL ),
};

static const struct snd_soc_dapm_widget mini210_dapm_playback_widgets[] = {
	SND_SOC_DAPM_HP(	"Headphone Jack",	NULL ),
	SND_SOC_DAPM_SPK(	"Speaker_L",		NULL ),
	SND_SOC_DAPM_SPK(	"Speaker_R",		NULL ),
};

static const struct snd_soc_dapm_route mini210_audio_map[] = {
	{ "Headphone Jack",	NULL,	"HP_L"		},
	{ "Headphone Jack",	NULL, 	"HP_R"		},
	{ "Speaker_L",		NULL, 	"SPK_LP"	}, 
	{ "Speaker_L",		NULL, 	"SPK_LN" 	}, 
	{ "Speaker_R",		NULL, 	"SPK_RP" 	}, 
	{ "Speaker_R",		NULL, 	"SPK_RN" 	}, 
	{ "LINPUT1",		NULL, 	"MICB"		},
	{ "MICB",			NULL, 	"Mic Jack"	},
};

static int mini210_wm8960_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	dprintk("+%s()\n", __FUNCTION__ );

	snd_soc_dapm_nc_pin(dapm, "RINPUT1");
	snd_soc_dapm_nc_pin(dapm, "LINPUT2");
	snd_soc_dapm_nc_pin(dapm, "RINPUT2");
	snd_soc_dapm_nc_pin(dapm, "OUT3");
	
	snd_soc_dapm_new_controls( dapm, mini210_dapm_capture_widgets, ARRAY_SIZE( mini210_dapm_capture_widgets ) );
	snd_soc_dapm_new_controls( dapm, mini210_dapm_playback_widgets, ARRAY_SIZE( mini210_dapm_playback_widgets ) );

	snd_soc_dapm_add_routes( dapm, mini210_audio_map, ARRAY_SIZE( mini210_audio_map ) );

	snd_soc_dapm_enable_pin(dapm, "Headphone Jack");
	snd_soc_dapm_enable_pin(dapm, "Mic Jack");
	snd_soc_dapm_enable_pin(dapm, "Speaker_L");
	snd_soc_dapm_enable_pin(dapm, "Speaker_R");
	
	snd_soc_dapm_disable_pin(dapm, "Line Input 3 (FM)");

	dprintk("*%s(): dapm sync start\n", __FUNCTION__ );
	snd_soc_dapm_sync( dapm );
	dprintk("*%s(): dapm sync end\n", __FUNCTION__ );

	dprintk("-%s()\n", __FUNCTION__ );
	return 0;
}

static struct snd_soc_dai_link mini210_dai = {
	.name = "MINI210",
	.stream_name = "WM8960 HiFi",
	.codec_name = "wm8960-codec",
	.platform_name = "samsung-audio",
	.cpu_dai_name = "samsung-i2s",
	.codec_dai_name = "wm8960-hifi",
	.init = mini210_wm8960_init,
	.ops = &mini210_wm8960_ops,
};

static struct snd_soc_card mini210_soc_card = {
	.name = "mini210",
	.dai_link = &mini210_dai,
	.num_links = 1,
};

static struct platform_device *mini210_snd_device;

static int __init mini210_audio_init(void)
{
	int ret;
	
	dprintk("Entered +%s()\n", __FUNCTION__ ); 

	mini210_snd_device = platform_device_alloc("soc-audio", -1);
	if ( !mini210_snd_device ){
		dprintk("-%s() : platform_device_alloc failed\n", __FUNCTION__ );
		return -ENOMEM;
	}

	platform_set_drvdata( mini210_snd_device, &mini210_soc_card );

	ret = platform_device_add( mini210_snd_device );
	if( ret ){
		platform_device_put( mini210_snd_device );
	}

	dprintk("-%s()\n", __FUNCTION__ );
	return ret;
}

static void __exit mini210_audio_exit(void)
{
	dprintk("+%s()\n", __FUNCTION__ );
	
	platform_device_unregister( mini210_snd_device );
	
	dprintk("-%s()\n", __FUNCTION__ );
}


module_init( mini210_audio_init );
module_exit( mini210_audio_exit );

//MODULE_AUTHOR("zoulz, happyzlz@sohu.com");
//MODULE_DESCRIPTION("ALSA SoC TINY210v2+WM8960");
MODULE_LICENSE("GPL");




