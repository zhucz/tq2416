/* linux/arch/arm/mach-s3c2416/mach-hanlin_v3c.c
 *
 * Copyright (c) 2009 Yauhen Kharuzhy <jekhor@gmail.com>,
 *	as part of OpenInkpot project
 * Copyright (c) 2009 Promwad Innovation Company
 *	Yauhen Kharuzhy <yauhen.kharuzhy@promwad.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/serial_s3c.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/mtd/partitions.h>
#include <linux/gpio.h>
#include <linux/fb.h>
#include <linux/delay.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <video/samsung_fimd.h>
#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include <mach/regs-gpio.h>
#include <mach/regs-lcd.h>
#include <mach/regs-s3c2443-clock.h>
#include <mach/gpio-samsung.h>

#include <linux/platform_data/leds-s3c24xx.h>
#include <linux/platform_data/i2c-s3c2410.h>

#include <plat/gpio-cfg.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <linux/platform_data/mtd-nand-s3c2410.h>
#include <plat/sdhci.h>
#include <linux/platform_data/usb-s3c2410_udc.h>
#include <linux/platform_data/s3c-hsudc.h>
#include <plat/samsung-time.h>

#include <plat/fb.h>
#include <plat/fb-s3c2416.h>


#include "common.h"
#include "common-smdk.h"


#if defined(CONFIG_DM9000)
#include <linux/dm9000.h>
#include <linux/irq.h>
#endif

static struct map_desc smdk2416_iodesc[] __initdata = {
	/* ISA IO Space map (memory space selected by A24) */

	{
		.virtual	= (u32)S3C24XX_VA_ISA_WORD,
		.pfn		= __phys_to_pfn(S3C2410_CS2),
		.length		= 0x10000,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (u32)S3C24XX_VA_ISA_WORD + 0x10000,
		.pfn		= __phys_to_pfn(S3C2410_CS2 + (1<<24)),
		.length		= SZ_4M,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (u32)S3C24XX_VA_ISA_BYTE,
		.pfn		= __phys_to_pfn(S3C2410_CS2),
		.length		= 0x10000,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (u32)S3C24XX_VA_ISA_BYTE + 0x10000,
		.pfn		= __phys_to_pfn(S3C2410_CS2 + (1<<24)),
		.length		= SZ_4M,
		.type		= MT_DEVICE,
	}
};

/*DM9000 Support*/
#if defined(CONFIG_DM9000)
#if 1//linux-3.1.0
static struct resource dm9000_resources[]={
	[0]={
		.start	= 0x20000300,
		.end	= 0x20000300 + 3,
		.flags	= IORESOURCE_MEM,
	},
	[1]={
		.start	= 0x20000300 + 8,
		.end	= 0x20000300 + 8 + 3,
		.flags	= IORESOURCE_MEM,
	},
	[2]={
		.start	= IRQ_EINT(4),
		.end	= IRQ_EINT(4),
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHEDGE,
	},
};
#endif

#if 0//linux-4.7.4
#define SMDK2416_DM9000_BASE	0x20000300
static struct resource dm9000_resources[]={
	[0]= DEFINE_RES_MEM(SMDK2416_DM9000_BASE,3),
	[1]= DEFINE_RES_MEM(SMDK2416_DM9000_BASE+8,3),
	[2]= DEFINE_RES_NAMED(IRQ_EINT(4),1,NULL,IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHEDGE),
};
#endif


static struct dm9000_plat_data smdk2416_dm9000_platdata={
	.flags	= DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM,
	.dev_addr[0]	= 0x00,
	.dev_addr[1]	= 0x40,
	.dev_addr[2]	= 0x5c,
	.dev_addr[3]	= 0x26,
	.dev_addr[4]	= 0x0a,
	.dev_addr[5]	= 0x5b,
};

static struct platform_device smdk2416_device_dm9000={
	.name	="dm9000",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(dm9000_resources),
	.resource	= dm9000_resources,
	.dev={
		.platform_data = &smdk2416_dm9000_platdata,
	}
};

#endif
/*End DM9000*/

/*-------------------------platform adc for s3c2416 -----zhuchengzhi TODO-*/

static struct platform_device s3c_device_s3c2416_adc = {
	.name = "s3c2416_adc",
	.id   = -1,
	.dev = {
		.parent = &s3c_device_adc.dev
	},
};
/*-----------------------------s3c2416 adc End----------------------------*/


#define UCON (S3C2410_UCON_DEFAULT	| \
		S3C2440_UCON_PCLK	| \
		S3C2443_UCON_RXERR_IRQEN)

#define ULCON (S3C2410_LCON_CS8 | S3C2410_LCON_PNONE)

#define UFCON (S3C2410_UFCON_RXTRIG8	| \
		S3C2410_UFCON_FIFOMODE	| \
		S3C2440_UFCON_TXTRIG16)

static struct s3c2410_uartcfg smdk2416_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	/* IR port */
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON | 0x50,
		.ufcon	     = UFCON,
	},
	[3] = {
		.hwport	     = 3,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	}
};

static void smdk2416_hsudc_gpio_init(void)
{
	s3c_gpio_setpull(S3C2410_GPH(14), S3C_GPIO_PULL_UP);
	s3c_gpio_setpull(S3C2410_GPF(2), S3C_GPIO_PULL_NONE);
	s3c_gpio_cfgpin(S3C2410_GPH(14), S3C_GPIO_SFN(1));
	s3c2410_modify_misccr(S3C2416_MISCCR_SEL_SUSPND, 0);
}

static void smdk2416_hsudc_gpio_uninit(void)
{
	s3c2410_modify_misccr(S3C2416_MISCCR_SEL_SUSPND, 1);
	s3c_gpio_setpull(S3C2410_GPH(14), S3C_GPIO_PULL_NONE);
	s3c_gpio_cfgpin(S3C2410_GPH(14), S3C_GPIO_SFN(0));
}

static struct s3c24xx_hsudc_platdata smdk2416_hsudc_platdata = {
	.epnum = 9,
	.gpio_init = smdk2416_hsudc_gpio_init,
	.gpio_uninit = smdk2416_hsudc_gpio_uninit,
};

static struct s3c_fb_pd_win smdk2416_fb_win[] = {
	[0] = {
		.default_bpp	= 16,
		.max_bpp		= 32,
		.xres           = 480,//800,
		.yres           = 272,//480,
		.virtual_x		= 544,
		.virtual_y		= 480,
	},
};

static struct fb_videomode smdk2416_lcd_timing = {
	.name           = "T43 480*272",
	.pixclock		= 100000,
	.left_margin	= 2,
	.right_margin	= 2,
	.upper_margin	= 2,
	.lower_margin	= 2,
	.hsync_len		= 41,
	.vsync_len		= 10,
	.xres           = 480,//800,
	.yres           = 272,//480,
};

static void s3c2416_fb_gpio_setup_24bpp(void)
{
	unsigned int gpio;

	for (gpio = S3C2410_GPC(1); gpio <= S3C2410_GPC(4); gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(2));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}

	for (gpio = S3C2410_GPC(8); gpio <= S3C2410_GPC(15); gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(2));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}

	for (gpio = S3C2410_GPD(0); gpio <= S3C2410_GPD(15); gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(2));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}
	printk("s3c2416_fb_gpio_setup_24bpp ..................\n");
}

static struct s3c_fb_platdata smdk2416_fb_platdata = {
	.win[0]		= &smdk2416_fb_win[0],
	.vtiming	= &smdk2416_lcd_timing,
	.setup_gpio	= s3c2416_fb_gpio_setup_24bpp,
	.vidcon0	= 0x000003b3,//VIDCON0_VIDOUT_RGB | VIDCON0_PNRMODE_RGB,
	.vidcon1	= 0x00000060,//VIDCON1_INV_HSYNC | VIDCON1_INV_VSYNC,
};

static struct s3c_sdhci_platdata smdk2416_hsmmc0_pdata __initdata = {
	.max_width		= 4,
	.cd_type		= S3C_SDHCI_CD_GPIO,
	.ext_cd_gpio		= S3C2410_GPF(1),
	.ext_cd_gpio_invert	= 1,
};

static struct s3c_sdhci_platdata smdk2416_hsmmc1_pdata __initdata = {
	.max_width		= 4,
	.cd_type		= S3C_SDHCI_CD_NONE,
};

/*-------------------TODO zhuchengzhi----------------------*/
static struct mys3c2416fb_hw  mys3c2416fb_reg_val = {
	.vidcon0 	  = 0x000000b0,//0x000003b3,    //0x4c800000
	.vidcon1      = 0x00000020,    //the video data is fetched at VCLK rising edge 
	.vidtcon0     = 0x00010109,    //Vertical
	.vidtcon1     = 0x00010128,    //Horizontal
	.vidtcon2     = 0x000881e0,    //size of display
	.wincon0 	  = 0x00010015,    //window 0 control register 565
	.wincon1 	  = 0x00010054,    //window 1 control register
	.vidosd0a 	  = 0x00000000, 
	.vidosd0b 	  = 0x000ef90f, 
	.vidosd1a 	  = 0x00000000, 
	.vidosd1b 	  = 0x000ef90f, 
	.vidosd1c 	  = 0x00fff000, 
//	.vidw00add0b0 = 0x00000000, 
//	.vidw00add0b1 = 0x00000000, 
//	.vidw01add0   = 0x00000000, 
//	.vidw00add1b0 = 0x00000000, 
//	.vidw00add1b1 = 0x00000000, 
//	.vidw01add1   = 0x00000000, 
//	.vidw00add2b0 = 0x000003c0, //
//	.vidw00add2b1 = 0x00000000, 
//	.vidw01add2   = 0x00000000, 
	.vidintcon    = 0x03fc302f, 
	.w1keycon0    = 0x02070307, 
	.w1keycon1 	  = 0x00f8fcf8, 
//	.w2keycon0 	  = 0x00000000, 
//	.w2keycon1 	  = 0x00000000, 
//	.w3keycon0 	  = 0x00000000, 
//	.w3keycon1 	  = 0x00000000, 
//	.w4keycon0 	  = 0x00000000, 
//	.w4keycon1 	  = 0x00000000, 
	.win0map 	  = 0x01f8fcf8, 
	.win1map 	  = 0x00000000, 
	.wpalcon 	  = 0x00000036, 
	.sysifcon0 	  = 0x00000000, 
	.sysifcon1 	  = 0x00000000, 
	.dithmode 	  = 0x00000000, 
	.sifccon0 	  = 0x00000000, 
	.sifccon1 	  = 0x00000000, 
	.sifccon2 	  = 0x00000000, 
	.cputrigcon2  = 0x00000000, 
	//.win0_plalette_ram;//0x4c800400-----0x4c8007fc  (0-255 address)
	//.win1_plalette_ram;//0x4c800800-----0x4c800bfc  (0-255 address)
};

static struct mys3c2416fb_display mys3c2416_fb_display = {
	/*LCD type name*/
	.name 		  = "T43 480*272",
	.type         = S3C2416_VIDCON0_TFT,
	/*Screen size*/
	.width	      = 480,
	.height 	  = 272, 
	/*Screen info*/
	.xres		  = 480,
	.yres		  = 272,
	.bpp		  = 16,
	.pixclock     = 100000,
	.left_margin  = 2,
	.right_margin = 2,
	.hsync_len	  = 41,
	.upper_margin = 2,
	.lower_margin = 2,
	.vsync_len    = 10,
	.mylcd_hw     = &mys3c2416fb_reg_val,

};

static struct mys3c2416fb_mach_info mys3c2416_fb_platdata = {
	.displays        = &mys3c2416_fb_display,
	.num_displays 	 = 1,
	.default_display = 0,

	.gpcup 		 	 = 0xaaaa02aa,
	.gpcup_mask  	 = 0x00000000,
	.gpccon 	 	 = 0xaaaa02a9,
	.gpccon_mask 	 = 0x00000000,

	.gpdup 		 	 = 0xaaaaaaaa,
	.gpdup_mask  	 = 0x00000000,
	.gpdcon 	 	 = 0xaaaaaaaa,
	.gpdcon_mask 	 = 0x00000000,

//	.lpcsel =, 
};
/*------------------------end------------------------------*/

static struct platform_device *smdk2416_devices[] __initdata = {
	&s3c_device_fb,
	&mys3c2416_device_lcd,
	&s3c_device_wdt,
	&s3c_device_ohci,
	&s3c_device_i2c0,
	&s3c_device_adc,
	&s3c_device_s3c2416_adc,
	&s3c_device_hsmmc0,
	&s3c_device_hsmmc1,
	&s3c_device_usb_hsudc,
	&s3c2443_device_dma,
	&smdk2416_device_dm9000,
};

static void __init smdk2416_init_time(void)
{
	s3c2416_init_clocks(12000000);
	samsung_timer_init();
}

static void __init smdk2416_map_io(void)
{
	s3c24xx_init_io(smdk2416_iodesc, ARRAY_SIZE(smdk2416_iodesc));
	s3c24xx_init_uarts(smdk2416_uartcfgs, ARRAY_SIZE(smdk2416_uartcfgs));
	samsung_set_timer_source(SAMSUNG_PWM3, SAMSUNG_PWM4);
}

static void __init smdk2416_machine_init(void)
{
	s3c_i2c0_set_platdata(NULL);

	mys3c2416_fb_set_platdata(&mys3c2416_fb_platdata);//TODO zhuchengzhi 2016-11-28

	s3c_fb_set_platdata(&smdk2416_fb_platdata);

	s3c_sdhci0_set_platdata(&smdk2416_hsmmc0_pdata);
	s3c_sdhci1_set_platdata(&smdk2416_hsmmc1_pdata);

	s3c24xx_hsudc_set_platdata(&smdk2416_hsudc_platdata);

	gpio_request(S3C2410_GPB(4), "USBHost Power");
	gpio_direction_output(S3C2410_GPB(4), 1);

	gpio_request(S3C2410_GPB(3), "Display Power");
	gpio_direction_output(S3C2410_GPB(3), 1);

	gpio_request(S3C2410_GPB(1), "Display Reset");
	gpio_direction_output(S3C2410_GPB(1), 1);

	platform_add_devices(smdk2416_devices, ARRAY_SIZE(smdk2416_devices));
	smdk_machine_init();
}

MACHINE_START(SMDK2416, "SMDK2416")
	/* Maintainer: Yauhen Kharuzhy <jekhor@gmail.com> */
	.atag_offset	= 0x100,

	.init_irq	= s3c2416_init_irq,
	.map_io		= smdk2416_map_io,
	.init_machine	= smdk2416_machine_init,
	.init_time	= smdk2416_init_time,
MACHINE_END
