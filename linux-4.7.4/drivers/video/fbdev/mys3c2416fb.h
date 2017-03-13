
/*
 * linux/drivers/video/s3c2416fb.h
 *	Copyright (c) 2016 Arnaud Patard
 *
 *  S3C2416 LCD Framebuffer Driver
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
*/

#ifndef __MYS3C2416FB_H
#define __MYS3C2416FB_H 
#include <plat/fb-s3c2416.h>

struct mys3c2416fb_info {
	struct device			*dev;
	struct clk				*clk;

	struct resource			*mem;
	void __iomem			*io;
	void __iomem			*irq_base;

	struct mys3c2416fb_hw	regs;

	unsigned long			clk_rate;
	unsigned int			palette_ready;

#ifdef CONFIG_CPU_FREQ
	struct notifier_block	freq_transition;
#endif

	/* keep these registers in case we need to re-write palette */
	u32						palette_buffer[256];
	u32						pseudo_pal[16];
};

#define PALETTE_BUFF_CLEAR (0x80000000)	/* entry is clear/invalid */


#define S3CFB_HSW				41
#define S3CFB_HBP				2
#define S3CFB_HFP				2

#define S3CFB_VSW				10
#define S3CFB_VBP				2
#define S3CFB_VFP				2

#define S3CFB_HRES				480
#define S3CFB_VRES				272
#define S3CFB_VFRAME_FREQ		66


#define LCD_COLOR16				4
#define PIXELBITS				16
#define LCD_BPP					LCD_COLOR16

#define S3CFB_IVCLK				0
#define S3CFB_IHSYNC			1
#define S3CFB_IVSYNC			1
#define S3CFB_IVDEN				0

#define S3CFB_PIXEL_CLOCK 		S3CFB_VFRAME_FREQ * \
								(S3CFB_HFP+S3CFB_HSW+S3CFB_HBP+S3CFB_HRES)* \
								(S3CFB_VFP+S3CFB_VSW+S3CFB_VBP+S3CFB_VRES)




//vidcon0
#define VIDCON0_S_RGB_IF				(0 << 22)
#define VIDCON0_S_RGB_PAR				(0 << 23)
#define VIDCON0_S_VCLK_GATING_OFF		(1 << 5)
#define VIDCON0_S_CLKDIR_DIVIDED		(1 << 4)
#define VIDCON0_S_CLKSEL_HCLK			(0 << 2)
#define VIDCON0_ENVID_ENABLE			(1 << 1)
#define VIDCON0_ENVID_F_ENABLE			(1 << 0)

#define VIDCON0_CLKVAL_F(x)				(((x)&0xff)<<6)


//vidcon1
#define	VIDCON1_S_HSYNC_INVERTED		(1 << 6)
#define VIDCON1_S_VSYNC_INVERTED		(1 << 5)

//VIDTCON0
#define VIDTCON0_VBPD(x)				(((x)&0xff)<<16)
#define VIDTCON0_VFPD(x)				(((x)&0xff)<< 8)
#define VIDTCON0_VSPW(x)				(((x)&0xff)<< 0)
//VIDTCON1
#define VIDTCON1_HBPD(x)				(((x)&0xff)<<16)
#define VIDTCON1_HFPD(x)				(((x)&0xff)<< 8)
#define VIDTCON1_HSPW(x)				(((x)&0xff)<< 0)

//VIDTCON2
#define VIDTCON2_LINEVAL(x)				(((x)&0x7ff)<<11)
#define VIDTCON2_HOZVAL(x)				(((x)&0x7ff)<< 0)


//WINCONX
#define WINCONx_BPPMODE_F_16BPP_565		(5 << 2)
#define WINCONx_ENWIN_F_ENABLE			(1 << 0)
#define	WINCONx_BLD_PIX_PIXEL			(1 << 6)
#define WINCONx_HAWSWP_ENABLE			(1 << 16)

//VIDOSD0A
#define VIDOSDxA_OSD_LTX_F(x)				(((x)&0x7ff)<<11)
#define VIDOSDxA_OSD_LTY_F(x)				(((x)&0x7ff)<< 0)
//VIDOSD0B
#define VIDOSDxB_OSD_RBX_F(x)				(((x)&0x7ff)<<11)
#define VIDOSDxB_OSD_RBY_F(x)				(((x)&0x7ff)<< 0)

//VIDWxADD2
#define VIDWxADDR2_OFFSIZE_F(x)			(((x)&0x1fff)<<13)
#define VIDWxADDR2_PAGEWIDTH_F(x)		(((x)&0x1fff)<< 0)


//WxKEYCON0
#define WxKEYCON0_KEYBLEN_ENABLE		(1 << 26)
#define WxKEYCON0_KEYEN_F_ENABLE		(1 << 25)

#define WxKEYCON0_COMPKEY(x)			(((x)&0xffffff)<<0)
int mys3c2416fb_init(void);

#endif

