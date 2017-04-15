/* linux/drivers/video/mys3c2416fb.c
 *	Copyright (c) 2004,2005 Arnaud Patard
 *	Copyright (c) 2004-2008 Ben Dooks
 *
 * S3C2416 LCD Framebuffer Driver
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
 * Driver based on skeletonfb.c, sa1100fb.c and others.
*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/io.h>

#include <asm/div64.h>

#include <asm/mach/map.h>
#include <mach/regs-lcd.h>
#include <mach/regs-gpio.h>
#include <mach/fb.h>

#ifdef CONFIG_PM
#include <linux/pm.h>
#endif

#include "mys3c2416fb.h"


/* Debugging stuff */
#ifdef CONFIG_FB_MYS3C2416_DEBUG
static int debug	= 1;
#else
static int debug;
#endif

#define dprintk(msg...) \
do { \
	if (debug) \
		pr_debug(msg); \
} while (0)


static int mys3c2416fb_suspend(struct platform_device *dev, pm_message_t state)
{
	return 0;
}


static int mys3c2416fb_resume(struct platform_device *dev)
{
	return 0;
}


/*
 *	mys3c2416fb_check_var():
 *	Get the video params out of 'var'. If a value doesn't fit, round it up,
 *	if it's too big, return -EINVAL.
 *
 */
static int mys3c2416fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct mys3c2416fb_info *fbi = info->par;
	struct mys3c2416fb_mach_info *mach_info = dev_get_platdata(fbi->dev);
	struct mys3c2416fb_display *display = NULL;
	struct mys3c2416fb_display *default_display = mach_info->displays + mach_info->default_display;

	int type = default_display->type;
	unsigned i;

	printk(KERN_INFO "check_var(var=%p, info=%p)\n", var, info);

	/*validate x/y resolution*/
	/*choose default mode if possible*/
	if(var->yres == default_display->yres &&
	   var->xres == default_display->xres &&
	   var->bits_per_pixel == default_display->bpp){
		display = default_display;
		printk(KERN_INFO "%s : %d display = %p \n",__func__,__LINE__,display);
	}else{
		for(i=0; i<mach_info->num_displays; i++){
			if(type == mach_info->displays[i].type &&
			   var->yres == mach_info->displays[i].yres &&
			   var->xres == mach_info->displays[i].xres &&
			   var->bits_per_pixel == mach_info->displays[i].bpp){
				display = mach_info->displays + i;

				printk(KERN_INFO "%s : %d display = %p \n",__func__,__LINE__,display);
				break;
			}
		}
	}

	if (!display) {
		printk(KERN_INFO "wrong resolution or depth %dx%d at %d bpp\n", var->xres, var->yres, var->bits_per_pixel);
		return -EINVAL;
	}

	/*it is always the size as the display*/
	var->xres_virtual = display->xres;
	var->yres_virtual = display->yres;
	var->height = display->height;
	var->width  = display->width;

	printk(KERN_INFO "%s : %d var->xres_virtual = %d ........ \n",__func__,__LINE__,var->xres_virtual);
	printk(KERN_INFO "%s : %d var->yres_virtual = %d ........ \n",__func__,__LINE__,var->yres_virtual);
	printk(KERN_INFO "%s : %d var->height = %d ........ \n",__func__,__LINE__,var->height);
	printk(KERN_INFO "%s : %d var->width = %d ........ \n",__func__,__LINE__,var->width);
	/*copy lcd settings*/
	var->pixclock = display->pixclock;
	var->left_margin = display->left_margin;
	var->right_margin = display->right_margin;
	var->upper_margin = display->upper_margin;
	var->lower_margin = display->lower_margin;
	var->vsync_len = display->vsync_len;
	var->hsync_len = display->hsync_len;


	printk(KERN_INFO "%s : %d var->pixclock = %d ........ \n",__func__,__LINE__,var->pixclock);
	printk(KERN_INFO "%s : %d var->left_margin = %d ........ \n",__func__,__LINE__,var->left_margin);
	printk(KERN_INFO "%s : %d var->right_margin = %d ........ \n",__func__,__LINE__,var->right_margin);
	printk(KERN_INFO "%s : %d var->upper_margin = %d ........ \n",__func__,__LINE__,var->upper_margin);
	printk(KERN_INFO "%s : %d var->lower_margin = %d ........ \n",__func__,__LINE__,var->lower_margin);
	printk(KERN_INFO "%s : %d var->vsync_len = %d ........ \n",__func__,__LINE__,var->vsync_len);
	printk(KERN_INFO "%s : %d var->hsync_len = %d ........ \n",__func__,__LINE__,var->hsync_len);


	fbi->regs.vidcon1 = display->mylcd_hw->vidcon1;//video control 2 register
	printk(KERN_INFO "%s : %d fbi->regs.vidcon1 = %#x ........ \n",__func__,__LINE__,fbi->regs.vidcon1);
	
	/*set display type*/
	fbi->regs.vidcon0 = display->mylcd_hw->vidcon0;
	printk(KERN_INFO "%s : %d fbi->regs.vidcon0 = %#x ........ \n",__func__,__LINE__,fbi->regs.vidcon0);

	var->transp.offset = 0;
	var->transp.length = 0;

	printk(KERN_INFO "%s : %d var->bits_per_pixel = %#x ........ \n",__func__,__LINE__,var->bits_per_pixel);

	/* set r/g/b positions */
	switch(var->bits_per_pixel){
		case 16:
			if (display->mylcd_hw->vidcon0 & S3C2416_VIDCON0_FRM565) {
				/* 16 bpp, 565 format */
				var->red.offset		= 11;
				var->green.offset	= 5;
				var->blue.offset	= 0;
				var->red.length		= 5;
				var->green.length	= 6;
				var->blue.length	= 5;
				printk(KERN_INFO "%s : %d 11 5 0 5 6 5 16bpp FM565........ \n",__func__,__LINE__);
			} else {
				/* 16 bpp, 5551 format */
				var->red.offset		= 11;
				var->green.offset	= 6;
				var->blue.offset	= 1;
				var->red.length		= 5;
				var->green.length	= 5;
				var->blue.length	= 5;
				printk(KERN_INFO "%s : %d 11 6 1 5 5 5 16bpp FM565........ \n",__func__,__LINE__);
			}
			break;
		default:
			break;
	}

	return 0;

}
/* mys3c2416fb_calc_pixclk()
 *
 * calculate divisor for clk->pixclk
 */
static unsigned int mys3c2416fb_calc_pixclk(struct mys3c2416fb_info *fbi, unsigned long pixclk)
{
	unsigned long clk = fbi->clk_rate;
	unsigned long long div;

	/* pixclk is in picoseconds, our clock is in Hz
	 *
	 * Hz -> picoseconds is / 10^-12
	 */

	printk(KERN_INFO "%s : %d clk=%ld div=%ld \n",__func__,__LINE__,clk,div);

	div = (unsigned long long)clk * pixclk;
	div >>= 12;			/* div / 2^12 */
	do_div(div, 625 * 625UL * 625); /* div / 5^12 */

	printk(KERN_INFO "pixclk %ld, divisor is %ld\n", pixclk, (long)div);

	return div;
}
/* mys3c2416fb_calculate_tft_lcd_regs
 *
 * calculate register values from var settings
 */
static void mys3c2416fb_calculate_tft_lcd_regs(const struct fb_info *info,
					     struct mys3c2416fb_hw *regs)
{
	const struct mys3c2416fb_info *fbi = info->par;
	const struct fb_var_screeninfo *var = &info->var;

	printk(KERN_INFO "%s : %d var->bits_per_pixel = %d ........ \n",__func__,__LINE__,var->bits_per_pixel);
	printk(KERN_INFO "%s : %d fbi->io = %p ........ \n",__func__,__LINE__,fbi->io);

	switch (var->bits_per_pixel) {
	case 1:
		regs->vidcon0 |= S3C2410_LCDCON1_TFT1BPP;
		break;
	case 2:
		regs->vidcon0 |= S3C2410_LCDCON1_TFT2BPP;
		break;
	case 4:
		regs->vidcon0 |= S3C2410_LCDCON1_TFT4BPP;
		break;
	case 8:
		regs->vidcon0 |= S3C2410_LCDCON1_TFT8BPP;
		regs->vidcon1 |= S3C2410_LCDCON5_BSWP |S3C2410_LCDCON5_FRM565;
		regs->vidcon1 &= ~S3C2410_LCDCON5_HWSWP;
		break;
	case 16:
		regs->wincon0 |= S3C2416_WINCON0_TFT16BPP | (1 << 0);
		regs->wincon0 &= ~S3C2416_WINCON0_BUFSEL;
		regs->wincon0 |= S3C2416_WINCON0_HAWSWP;


		regs->wincon1 = (5<<2)|(1<<16)|(1<<6);

		printk("%s: %d case 16 \n",__func__,__LINE__);
		break;
	case 32:
		regs->vidcon0 |= S3C2410_LCDCON1_TFT24BPP;
		regs->vidcon1 &= ~(S3C2410_LCDCON5_BSWP |S3C2410_LCDCON5_HWSWP |S3C2410_LCDCON5_BPP24BL);
		break;
	default:
		/* invalid pixel depth */
		printk(KERN_INFO "invalid bpp %d\n", var->bits_per_pixel);
	}
	/* update X/Y info */
	printk(KERN_INFO "%s : %d setting vert: up=%d, low=%d, sync=%d\n",__func__,__LINE__,
		var->upper_margin, var->lower_margin, var->vsync_len);

	printk(KERN_INFO "%s : %d setting horz: lft=%d, rt=%d, sync=%d\n",__func__,__LINE__,
		var->left_margin, var->right_margin, var->hsync_len);



	regs->vidosd0a = 0x0;  //左边坐标
	regs->vidosd0b = ((480-1)<<11) | ((272-1)<<0);//右边坐标

	regs->vidosd1a = 0x0;  //左边坐标
	regs->vidosd1b = ((480-1)<<11) | ((272-1)<<0);//右边坐标


	regs->vidtcon2 = ((272-1)<<11) | ((480-1)<<0);



	writel(regs->vidosd0a,fbi->io + S3C2416_VIDOSD0A);
	writel(regs->vidosd0b,fbi->io + S3C2416_VIDOSD0B);

	regs->vidtcon0 = S3C2416_VIDTCON0_VBPD(var->upper_margin - 1) |
					  S3C2416_VIDTCON0_VFPD(var->lower_margin - 1) |
					  S3C2416_VIDTCON0_VSPW(var->vsync_len - 1);

	regs->vidtcon1 = S3C2416_VIDTCON1_VBPD(var->left_margin - 1) |
					  S3C2416_VIDTCON1_VFPD(var->right_margin - 1) |
					  S3C2416_VIDTCON1_VSPW(var->hsync_len - 1);

	
	printk(KERN_INFO "%s : %d regs->vidtcon0 = %#x\n",__func__,__LINE__,regs->vidtcon0);
	printk(KERN_INFO "%s : %d regs->vidtcon1 = %#x\n",__func__,__LINE__,regs->vidtcon1);

	printk(KERN_INFO "%s : %d regs->wincon0 = %#x\n",__func__,__LINE__,regs->wincon0);
	printk(KERN_INFO "%s : %d regs->wincon1 = %#x\n",__func__,__LINE__,regs->wincon1);
	printk(KERN_INFO "%s : %d regs->vidcon0 = %#x\n",__func__,__LINE__,regs->vidcon0);
	printk(KERN_INFO "%s : %d regs->vidcon1 = %#x\n",__func__,__LINE__,regs->vidcon1);
	printk(KERN_INFO "%s : %d regs->vidtcon2 = %#x\n",__func__,__LINE__, regs->vidtcon2);



	writel(regs->vidtcon0,fbi->io + S3C2416_VIDTCON0);
	writel(regs->vidtcon1,fbi->io + S3C2416_VIDTCON1);

	
	writel(regs->wincon0,fbi->io + S3C2416_WINCON0);
	writel(regs->wincon1,fbi->io + S3C2416_WINCON1);
	writel(regs->vidcon0,fbi->io + S3C2416_VIDCON0);
	writel(regs->vidcon1,fbi->io + S3C2416_VIDCON1);
	writel(regs->vidtcon2,fbi->io + S3C2416_VIDTCON2);


	printk(KERN_INFO "%s : %d vidtcon0 = %#x\n",__func__,__LINE__,readl(fbi->io + S3C2416_VIDTCON0));
	printk(KERN_INFO "%s : %d vidosd0a = %#x\n",__func__,__LINE__,readl(fbi->io + S3C2416_VIDOSD0A));
	printk(KERN_INFO "%s : %d vidosd0b = %#x\n",__func__,__LINE__,readl(fbi->io + S3C2416_VIDOSD0B));

}


/* mys3c2416fb_set_lcdaddr
 *
 * initialise lcd controller address pointers
 */
static void mys3c2416fb_set_lcdaddr(struct fb_info *info)
{
	unsigned long saddr1, saddr2, saddr3;
	struct mys3c2416fb_info *fbi = info->par;
	void __iomem *regs = fbi->io;


//	regs->vidw00add0b0 = info->fix.smem_start
//	regs->vidw00add1b0 = info->fix.smem_start + (info->fix.line_length * info->var.yres)
//	regs->vidw00add2b0 = S3C2416_OFFSIZE(0) | S3C2416_PAGEWIDTH((info->fix.line_length/2)&0x7ff);
//	saddr1 = regs->vidw00add0b0;
//	saddr2 = regs->vidw00add1b0;
//	saddr3 = regs->vidw00add2b0;


	saddr1 = info->fix.smem_start;
	saddr2 = info->fix.smem_start + (info->fix.line_length * info->var.yres);
	saddr3 = (0<<13) | ((480*2) << 0);//S3C2416_OFFSIZE(0) | S3C2416_PAGEWIDTH((info->fix.line_length/2)&0x7ff);

	printk(KERN_INFO "info->fix.smem_start = %#x \n",info->fix.smem_start);
	printk(KERN_INFO "info->fix.line_length = %#x \n",info->fix.line_length);
	printk(KERN_INFO "info->fix.smem_start = %#x \n",info->var.yres);
	printk(KERN_INFO "regs = %p \n",fbi->io);

	printk(KERN_INFO "VIDW00ADD0B0 = 0x%08lx\n", saddr1);
	printk(KERN_INFO "VIDW00ADD1B0 = 0x%08lx\n", saddr2);
	printk(KERN_INFO "VIDW00ADD2B0 = 0x%08lx\n", saddr3);

	writel(saddr1, regs + S3C2416_VIDW00ADD0B0);
	writel(saddr2, regs + S3C2416_VIDW00ADD1B0);
	writel(saddr3, regs + S3C2416_VIDW00ADD2B0);

#if 0
	saddr1  = info->fix.smem_start >> 1;
	saddr2  = info->fix.smem_start;
	saddr2 += info->fix.line_length * info->var.yres;
	saddr2 >>= 1;

	saddr3 = S3C2410_OFFSIZE(0) |  S3C2410_PAGEWIDTH((info->fix.line_length / 2) & 0x3ff);

	dprintk("LCDSADDR1 = 0x%08lx\n", saddr1);
	dprintk("LCDSADDR2 = 0x%08lx\n", saddr2);
	dprintk("LCDSADDR3 = 0x%08lx\n", saddr3);

	writel(saddr1, regs + S3C2410_LCDSADDR1);
	writel(saddr2, regs + S3C2410_LCDSADDR2);
	writel(saddr3, regs + S3C2410_LCDSADDR3);
#endif
}

/* mys3c2416fb_activate_var
 *
 * activate (set) the controller from the given framebuffer
 * information
 */
static void mys3c2416fb_activate_var(struct fb_info *info)
{
	struct mys3c2416fb_info *fbi = info->par;
	void __iomem *regs = fbi->io;
	int type = fbi->regs.vidcon0 & S3C2416_VIDCON0_FRM565;
	struct fb_var_screeninfo *var = &info->var;
	int clkdiv;


	clkdiv = DIV_ROUND_UP(mys3c2416fb_calc_pixclk(fbi, var->pixclock), 2);
	printk(KERN_INFO "%s: var->xres  = %d\n", __func__, var->xres);
	printk(KERN_INFO "%s: var->yres  = %d\n", __func__, var->yres);
	printk(KERN_INFO "%s: var->bpp   = %d\n", __func__, var->bits_per_pixel);

	if(type == S3C2416_VIDCON0_FRM565){
		mys3c2416fb_calculate_tft_lcd_regs(info, &fbi->regs);
		--clkdiv;
		if (clkdiv < 0){
			clkdiv = 0;
		}
		printk(KERN_INFO "%s : %d type == S3C2416_VIDCON0_TFT \n",__func__,__LINE__);
	}else{
		mys3c2416fb_calculate_tft_lcd_regs(info, &fbi->regs);
		--clkdiv;
		if (clkdiv < 2){
			clkdiv = 2;
		}
		printk(KERN_INFO "%s : %d type == S3C2416_VIDCON0_TFT \n",__func__,__LINE__);
	}

	printk(KERN_INFO "%s : %d clkdiv = %d \n",__func__,__LINE__,clkdiv);
	fbi->regs.vidcon0 |=  S3C2416_VIDTCON0_CLKVAL(clkdiv);

	/* write new registers */

	printk("new register set:\n");
	printk("vidcon1 [2] = 0x%08lx\n", fbi->regs.vidcon1);
	printk("vidtcon0[3] = 0x%08lx\n", fbi->regs.vidtcon0);
	printk("vidtcon1[4] = 0x%08lx\n", fbi->regs.vidtcon1);
	printk("vidtcon2[5] = 0x%08lx\n", fbi->regs.vidtcon2);

	/*set lcd address pointers*/
	mys3c2416fb_set_lcdaddr(info);

	fbi->regs.vidcon0 |= S3C2416_VIDCON0_ENVID;
	writel(fbi->regs.vidcon0, regs + S3C2416_VIDCON0);

}


static int mys3c2416fb_set_par(struct fb_info *info)
{
	struct fb_var_screeninfo *var = &info->var;/
	printk(KERN_INFO "%s : %d var->bits_per_pixel = %d ........ \n",__func__,__LINE__,var->bits_per_pixel);
	switch(var->bits_per_pixel){
		case 32:
		case 16:
		case 12:
			info->fix.visual = FB_VISUAL_TRUECOLOR;
			break;
		case 1:
			info->fix.visual = FB_VISUAL_MONO01;
			break;
		default:
			info->fix.visual = FB_VISUAL_PSEUDOCOLOR;
			break;
	}
	info->fix.line_length = ((var->xres_virtual * var->bits_per_pixel) / 8);
	printk(KERN_INFO "%s : %d info->fix.line_length = %d ........ \n",__func__,__LINE__,info->fix.line_length);

	/*activate this new configuration*/
	mys3c2416fb_activate_var(info);

	return 0;
}



/* mys3c2416fb_lcd_enable
 *
 * shutdown the lcd controller
 */
static void mys3c2416fb_lcd_enable(struct mys3c2416fb_info *fbi, int enable)
{
	unsigned long flags;

	local_irq_save(flags);

	if (enable){
		fbi->regs.vidcon0 |= S3C2416_VIDCON0_ENVID;
		printk(KERN_INFO "%s : %d enable fbi->regs.vidcon0 = %#x \n",__func__,__LINE__,fbi->regs.vidcon0);
	}else{
		fbi->regs.vidcon0 &= ~S3C2416_VIDCON0_ENVID;
		printk(KERN_INFO "%s : %d disenable fbi->regs.vidcon0 = %#x \n",__func__,__LINE__,fbi->regs.vidcon0);
	}
	writel(fbi->regs.vidcon0, fbi->io + S3C2416_VIDCON0);

	printk(KERN_INFO "%s : %d  fbi->io = %p \n",__func__,__LINE__,fbi->io);
	local_irq_restore(flags);
}



/*
 *  mys3c2416fb_blank
 *	@blank_mode: the blank mode we want.
 *	@info: frame buffer structure that represents a single frame buffer
 *
 *	Blank the screen if blank_mode != 0, else unblank. Return 0 if
 *	blanking succeeded, != 0 if un-/blanking failed due to e.g. a
 *	video mode which doesn't support it. Implements VESA suspend
 *	and powerdown modes on hardware that supports disabling hsync/vsync:
 *
 *	Returns negative errno on error, or zero on success.
 *
 */
static int mys3c2416fb_blank(int blank_mode, struct fb_info *info)
{
	struct mys3c2416fb_info *fbi = info->par;
	void __iomem *tpal_reg = fbi->io;
	unsigned long val_wpalcon = 0;
	

	printk(KERN_INFO "%s : %d blank(mode=%d, info=%p)\n", __func__,__LINE__,blank_mode, info);

	tpal_reg += S3C2416_WPALCON;
	val_wpalcon = readl(tpal_reg);


	if (blank_mode == FB_BLANK_POWERDOWN){
		mys3c2416fb_lcd_enable(fbi, 0);
		printk(KERN_INFO "lcd disabled \n");
	}else{
		mys3c2416fb_lcd_enable(fbi, 1);
		printk(KERN_INFO "lcd enable \n");
	}

	if (blank_mode == FB_BLANK_UNBLANK){
		val_wpalcon &= ~(1 << 9);
		writel(val_wpalcon, tpal_reg);
		printk(KERN_INFO "%s : %d setting TPAL to output \n",__func__,__LINE__);
	}else {
		printk(KERN_INFO "%s : %d setting TPAL to output \n",__func__,__LINE__);
		val_wpalcon |= (1 << 9);
		writel(val_wpalcon, tpal_reg);
	}

	return 0;
}


/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan,
					 struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static void schedule_palette_update(struct mys3c2416fb_info *fbi,
				    unsigned int regno, unsigned int val)
{
	unsigned long flags;
	unsigned long irqen;
	void __iomem *irq_base = fbi->irq_base;

	local_irq_save(flags);

	fbi->palette_buffer[regno] = val;

	printk(KERN_INFO "%s : %d fbi->palette_ready = %d \n",__func__,__LINE__,fbi->palette_ready);
	printk(KERN_INFO "%s : %d val = %d \n",__func__,__LINE__,val);

	if (!fbi->palette_ready) {
		fbi->palette_ready = 1;

		/* enable IRQ */
		irqen = readl(irq_base + S3C2416_VIDINTCON);
		irqen &= ~S3C2416_LCD_INTEN;
		writel(irqen, irq_base + S3C2416_VIDINTCON);
		printk(KERN_INFO "%s : %d irqen = %ld \n",__func__,__LINE__,irqen);
	}

	local_irq_restore(flags);
}

static int mys3c2416fb_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	struct mys3c2416fb_info *fbi = info->par;
	void __iomem *regs = fbi->io;
	unsigned int val;

	printk(KERN_INFO "%s : %d setcol: regno=%d, rgb=%d,%d,%d \n",__func__,__LINE__, regno, red, green, blue); 
	printk(KERN_INFO "%s : %d info->fix.visual = %d \n",__func__,__LINE__, info->fix.visual); 

	switch (info->fix.visual) {
	case FB_VISUAL_TRUECOLOR: // 2
		/* true-colour, use pseudo-palette */

		if (regno < 16) {
			u32 *pal = info->pseudo_palette;

			val  = chan_to_field(red,   &info->var.red);
			val |= chan_to_field(green, &info->var.green);
			val |= chan_to_field(blue,  &info->var.blue);

			pal[regno] = val;

			dprintk(KERN_INFO "%s : %d info->var.red   = %#x \n",__func__,__LINE__, info->var.red.length ); 
			dprintk(KERN_INFO "%s : %d info->var.green = %#x \n",__func__,__LINE__, info->var.green.length); 
			dprintk(KERN_INFO "%s : %d info->var.blue  = %#x \n",__func__,__LINE__, info->var.blue.length); 


			dprintk(KERN_INFO "%s : %d info->var.red.offset   = %#x \n",__func__,__LINE__, info->var.red.offset ); 
			dprintk(KERN_INFO "%s : %d info->var.green.offset = %#x \n",__func__,__LINE__, info->var.green.offset); 
			dprintk(KERN_INFO "%s : %d info->var.blue.offset  = %#x \n",__func__,__LINE__, info->var.blue.offset); 
			dprintk(KERN_INFO "%s : %d VAL = %#x \n",__func__,__LINE__, val); 

		}
		break;

	case FB_VISUAL_PSEUDOCOLOR: //3 
		if (regno < 256) {
			/* currently assume RGB 5-6-5 mode */

			val  = (red   >>  0) & 0xf800;
			val |= (green >>  5) & 0x07e0;
			val |= (blue  >> 11) & 0x001f;

			writel(val, regs + S3C2416_WIN0_PALETTE(regno));
			schedule_palette_update(fbi, regno, val);

			printk(KERN_INFO "%s : %d VAL = %#x \n",__func__,__LINE__, val); 
		}//TODO zhuchengzhi 2016-12-3
		break;

	default:
		return 1;	/* unknown type */
	}
	
	printk("%s : %d \n",__func__,__LINE__);
	return 0;
}


static int mys3c2416fb_debug_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", debug ? "on" : "off");
}

static int mys3c2416fb_debug_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t len)
{
	if (len < 1)
		return -EINVAL;

	if (strncasecmp(buf, "on", 2) == 0 || strncasecmp(buf, "1", 1) == 0) {
		debug = 1;
		dev_dbg(dev, "s3c2416fb: Debug On");
	} else if (strncasecmp(buf, "off", 3) == 0 ||  strncasecmp(buf, "0", 1) == 0) {
		debug = 0;
		dev_dbg(dev, "s3c2416fb: Debug Off");
	} else {
		return -EINVAL;
	}

	return len;
}

static DEVICE_ATTR(debug, 0664, mys3c2416fb_debug_show, mys3c2416fb_debug_store);

static struct fb_ops mys3c2416fb_ops = {
	.owner			= THIS_MODULE,
	.fb_check_var	= mys3c2416fb_check_var,
	.fb_set_par		= mys3c2416fb_set_par,
	.fb_blank	    = mys3c2416fb_blank,
	.fb_setcolreg	= mys3c2416fb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,

};

static void mys3c2416fb_write_palette(struct mys3c2416fb_info *fbi)
{
	unsigned int i;
	void __iomem *regs = fbi->io;

	fbi->palette_ready = 0;

	for (i = 0; i < 256; i++) {
		unsigned long ent = fbi->palette_buffer[i];
		if (ent == PALETTE_BUFF_CLEAR)
			continue;

		writel(ent, regs + S3C2416_WIN0_PALETTE(i));

		/* it seems the only way to know exactly
		 * if the palette wrote ok, is to check
		 * to see if the value verifies ok
		 */

		if (readw(regs + S3C2416_WIN0_PALETTE(i)) == ent)
			fbi->palette_buffer[i] = PALETTE_BUFF_CLEAR;
		else
			fbi->palette_ready = 1;   /* retry */
	}
}

static irqreturn_t mys3c2416fb_irq(int irq, void *dev_id)
{
	struct mys3c2416fb_info *fbi = dev_id;
	void __iomem *irq_base = fbi->irq_base;
	unsigned long lcdirq = readl(irq_base + S3C24XX_LCDINTPND);


	printk(KERN_INFO "%s : %d irq_base = %p \n",__func__,__LINE__,irq_base);
	printk(KERN_INFO "%s : %d lcdirq = %ld \n",__func__,__LINE__,lcdirq);

	if (lcdirq & S3C2410_LCDINT_FRSYNC) {
		if (fbi->palette_ready){
			mys3c2416fb_write_palette(fbi);
		}
		writel(S3C2410_LCDINT_FRSYNC, irq_base + S3C24XX_LCDINTPND);
		writel(S3C2410_LCDINT_FRSYNC, irq_base + S3C24XX_LCDSRCPND);
	}

	return IRQ_HANDLED;
}

/*
 * mys3c2416fb_map_video_memory():
 *	Allocates the DRAM memory for the frame buffer.  This buffer is
 *	remapped into a non-cached, non-buffered, memory region to
 *	allow palette and pixel writes to occur without flushing the
 *	cache.  Once this area is remapped, all virtual memory
 *	access to the video memory should occur at the new region.
 */
static int mys3c2416fb_map_video_memory(struct fb_info *info)
{

	printk(KERN_INFO "%s : %d \n",__func__,__LINE__);
	struct mys3c2416fb_info *fbi = info->par;
	dma_addr_t map_dma;


	unsigned map_size = PAGE_ALIGN(info->fix.smem_len);


	printk(KERN_INFO "%s : %d info->fix.smem_len = %ld \n",__func__,__LINE__,info->fix.smem_len);
	printk(KERN_INFO "map_video_memory(fbi=%p) map_size %u\n", fbi, map_size);


	info->screen_base = dma_alloc_wc(fbi->dev, map_size, &map_dma, GFP_KERNEL);
	printk(KERN_INFO "%s : %d info->screen_base = %p \n",__func__,__LINE__, info->screen_base);

	if (info->screen_base) {
		/* prevent initial garbage on screen */
		printk(KERN_INFO "map_video_memory: clear %p:%08x\n",info->screen_base, map_size);
		memset(info->screen_base, 0x00, map_size);

		info->fix.smem_start = map_dma;

		printk(KERN_INFO "map_video_memory: dma=%08lx cpu=%p size=%08x\n",info->fix.smem_start, info->screen_base, map_size);
	}

	return info->screen_base ? 0 : -ENOMEM;
}


static inline void mys3c2416fb_unmap_video_memory(struct fb_info *info)
{
	struct mys3c2416fb_info *fbi = info->par;

	dma_free_wc(fbi->dev, PAGE_ALIGN(info->fix.smem_len), info->screen_base, info->fix.smem_start);
}

static inline void modify_gpio(void __iomem *reg, unsigned long set, unsigned long mask)
{
	unsigned long tmp;

	tmp = readl(reg) & ~mask;
	writel(tmp | set, reg);
}



/*
 * mys3c2416fb_init_registers - Initialise all LCD-related registers
 */
#if 1
static int mys3c2416fb_init_registers(struct fb_info *info)
{
	struct mys3c2416fb_info *fbi = info->par;
	struct mys3c2416fb_mach_info *mach_info = dev_get_platdata(fbi->dev);
	unsigned long flags;
	void __iomem *regs = fbi->io;
	void __iomem *tpal;

	unsigned long wpalcon_val = 0;

	printk("%s : %d regs = %p \n",__func__,__LINE__,regs);

	
	tpal = regs + S3C2416_WPALCON;
	wpalcon_val = readl(tpal);
	wpalcon_val	|= (1 << 9) | (6 << 3) | (6 << 0);
	writel(wpalcon_val, tpal);

	/* Initialise LCD with values from haret */

	printk("%s : %d gpcup 	    = %#x \n",__func__,__LINE__,mach_info->gpcup);
	printk("%s : %d gpcup_mask  = %#x \n",__func__,__LINE__,mach_info->gpcup_mask);
	printk("%s : %d gpccon 	    = %#x \n",__func__,__LINE__,mach_info->gpccon);
	printk("%s : %d gpccon_mask = %#x \n",__func__,__LINE__,mach_info->gpccon_mask);
	printk("%s : %d gpdup 		= %#x \n",__func__,__LINE__,mach_info->gpdup);
	printk("%s : %d gpdup_mask  = %#x \n",__func__,__LINE__,mach_info->gpdup_mask);
	printk("%s : %d gpdcon 		= %#x \n",__func__,__LINE__,mach_info->gpdcon);
	printk("%s : %d gpdup_mask 	= %#x \n",__func__,__LINE__,mach_info->gpdcon_mask);


	local_irq_save(flags);
	/* modify the gpio(s) with interrupts set (bjd) */
	modify_gpio(S3C2416_GPCUP,  mach_info->gpcup,  mach_info->gpcup_mask);
	modify_gpio(S3C2416_GPCCON, mach_info->gpccon, mach_info->gpccon_mask);
	modify_gpio(S3C2416_GPDUP,  mach_info->gpdup,  mach_info->gpdup_mask);
	modify_gpio(S3C2416_GPDCON, mach_info->gpdcon, mach_info->gpdcon_mask);
	local_irq_restore(flags);


//	printk(KERN_INFO "replacing TPAL %08x\n", readl(tpal));
	/* ensure temporary palette disabled */
//	writel(0x00, tpal);

	wpalcon_val = readl(tpal);
	wpalcon_val &= ~(1 << 9);
	writel(wpalcon_val, tpal);

	return 0;
}

#else
static int mys3c2416fb_init_registers(struct fb_info *info)
{
	struct mys3c2416fb_info *fbi = info->par;
	struct mys3c2416fb_mach_info *mach_info = dev_get_platdata(fbi->dev);
	unsigned long flags;
	void __iomem *regs = fbi->io;
	void __iomem *tpal;
	void __iomem *lpcsel;

	unsigned long tmp_gpcup = 0;
	unsigned long tmp_gpccon = 0;
	unsigned long tmp_gpdup = 0;
	unsigned long tmp_gpdcon = 0;
	unsigned long tmp_gpcdat = 0;

	unsigned long tmp_vidcon0;
	unsigned long tmp_vidcon1;
	unsigned long tmp_vidtcon0;
	unsigned long tmp_vidtcon1;
	unsigned long tmp_vidtcon2;
	unsigned long tmp_wincon0;
	unsigned long tmp_wincon1;
	unsigned long tmp_vidosd0a;
	unsigned long tmp_vidosd0b;
	unsigned long tmp_vidosd1a;
	unsigned long tmp_vidosd1b;
	unsigned long tmp_vidosd1c;
	unsigned long tmp_vidintcon;

	unsigned long tmp_w1keycon0;
	unsigned long tmp_w1keycon1;


	unsigned long tmp_win0map;
	unsigned long tmp_wpalcon;
	

	tpal = regs + S3C2416_WPALCON;
//	lpcsel = regs + S3C2410_LPCSEL;

	/* Initialise LCD with values from haret */

	local_irq_save(flags);

	/* modify the gpio(s) with interrupts set (bjd) */

	modify_gpio(S3C2416_GPCUP,  mach_info->gpcup,  mach_info->gpcup_mask);
	modify_gpio(S3C2416_GPCCON, mach_info->gpccon, mach_info->gpccon_mask);
	modify_gpio(S3C2416_GPDUP,  mach_info->gpdup,  mach_info->gpdup_mask);
	modify_gpio(S3C2416_GPDCON, mach_info->gpdcon, mach_info->gpdcon_mask);



    tmp_vidcon0 	 = readl(regs + S3C2416_VIDCON0);
	tmp_vidcon1 	 = readl(regs + S3C2416_VIDCON1);
	tmp_vidtcon0 	 = readl(regs + S3C2416_VIDTCON0);
	tmp_vidtcon1 	 = readl(regs + S3C2416_VIDTCON1);
	tmp_vidtcon2 	 = readl(regs + S3C2416_VIDTCON2);
	tmp_wincon0 	 = readl(regs + S3C2416_WINCON0);
	tmp_wincon1 	 = readl(regs + S3C2416_WINCON1);
	tmp_vidosd0a 	 = readl(regs + S3C2416_VIDOSD0A);
	tmp_vidosd0b 	 = readl(regs + S3C2416_VIDOSD0B);
	tmp_vidosd1a 	 = readl(regs + S3C2416_VIDOSD1A);
	tmp_vidosd1b 	 = readl(regs + S3C2416_VIDOSD1B);
	tmp_vidosd1c 	 = readl(regs + S3C2416_VIDOSD1C);
	tmp_w1keycon0 	 = readl(regs + S3C2416_W1KEYCON0);
	tmp_w1keycon1 	 = readl(regs + S3C2416_W1KEYCON1);
	tmp_win0map		 = readl(regs + S3C2416_WIN0MAP);
	tmp_wpalcon		 = readl(regs + S3C2416_WPALCON);

	tmp_vidcon0 	 = 0x000003b3;
	tmp_vidcon1 	 = 0x00000060;
	tmp_vidtcon0	 = 0x00010109;
	tmp_vidtcon1	 = 0x00010128;
	tmp_vidtcon2	 = 0x000881e0;
	tmp_wincon0	  	 = 0x00010015;
	tmp_wincon1	  	 = 0x00010054;
	tmp_vidosd0a	 = 0x00000000;
	tmp_vidosd0b	 = 0x000ef90f;
	tmp_vidosd1a	 = 0x00000000;
	tmp_vidosd1b	 = 0x000ef90f;

	tmp_vidosd1c	 = 0x00fff000;
//	tmp_vidw00add0b0 =;
//	tmp_vidw00add1b0 =;
//	tmp_vidw00add2b0 =;
	tmp_vidintcon	 = 0x03fc302f ;
	tmp_w1keycon0    = 0x02070307;
	tmp_w1keycon1    = 0x00f8fcf8;
	tmp_win0map	 	 = 0x01f8fcf8;
	tmp_wpalcon	     = 0x00000036;//(1 << 9)


	writel(tmp_vidcon0,regs + S3C2416_VIDCON0);
	writel(tmp_vidcon1 ,regs + S3C2416_VIDCON1);
	writel(tmp_vidtcon0,regs + S3C2416_VIDTCON0);
	writel(tmp_vidtcon1,regs + S3C2416_VIDTCON1);
	writel(tmp_vidtcon2,regs + S3C2416_VIDTCON2);
	writel(tmp_wincon0,regs + S3C2416_WINCON0);
	writel(tmp_wincon1,regs + S3C2416_WINCON1);
	writel(tmp_vidosd0a,regs + S3C2416_VIDOSD0A);
	writel(tmp_vidosd0b,regs + S3C2416_VIDOSD0B);
	writel(tmp_vidosd1a,regs + S3C2416_VIDOSD1A);
	writel(tmp_vidosd1b,regs + S3C2416_VIDOSD1B);
	writel(tmp_vidosd1c,regs + S3C2416_VIDOSD1C);
//	writel(tmp_vidw00add0b0,regs + S3C2416_VIDW00ADD0B0);
//	writel(tmp_vidw00add1b0,regs + S3C2416_VIDW00ADD1B0);
//	writel(tmp_vidw00add2b0,regs + S3C2416_VIDW00ADD2B0);
	writel(tmp_w1keycon0,regs + S3C2416_W1KEYCON0);
	writel(tmp_w1keycon1,regs + S3C2416_W1KEYCON1);
	writel(tmp_win0map,regs + S3C2416_WIN0MAP);
	writel(tmp_wpalcon,regs + S3C2416_WPALCON);

	local_irq_restore(flags);

	tmp_gpcup  = readl(S3C2416_GPCUP);
	tmp_gpccon = readl(S3C2416_GPCCON);
	tmp_gpdup  = readl(S3C2416_GPDUP);
	tmp_gpdcon = readl(S3C2416_GPDCON);
	tmp_gpcdat = readl(S3C2416_GPCDAT);
//	tmp_gpcdat |= (1 << 0);
//	writel(tmp_gpcdat,S3C2416_GPCDAT);


	dprintk(KERN_INFO "%s : %d tmp_gpcup = %#x  \n",__func__,__LINE__,tmp_gpcup);
	dprintk(KERN_INFO "%s : %d tmp_gpccon = %#x \n",__func__,__LINE__,tmp_gpccon);
	dprintk(KERN_INFO "%s : %d tmp_gpdup = %#x  \n",__func__,__LINE__,tmp_gpdup);
	dprintk(KERN_INFO "%s : %d tmp_gpdcon = %#x \n",__func__,__LINE__,tmp_gpdcon);




	printk(KERN_INFO "LPCSEL    = 0x%08lx\n", mach_info->lpcsel);
//	writel(mach_info->lpcsel, lpcsel);

	printk(KERN_INFO "replacing TPAL %08x\n", readl(tpal));

	/* ensure temporary palette disabled */
	writel(0x00, tpal);

	return 0;
}
#endif


#ifdef CONFIG_CPU_FREQ
static int mys3c2416fb_cpufreq_transition(struct notifier_block *nb,
					unsigned long val, void *data)
{
	struct mys3c2416fb_info *info;
	struct fb_info *fbinfo;
	long delta_f;

	info = container_of(nb, struct mys3c2416fb_info, freq_transition);
	fbinfo = platform_get_drvdata(to_platform_device(info->dev));

	/* work out change, <0 for speed-up */
	delta_f = info->clk_rate - clk_get_rate(info->clk);

	if ((val == CPUFREQ_POSTCHANGE && delta_f > 0) ||
	    (val == CPUFREQ_PRECHANGE && delta_f < 0)) {
		info->clk_rate = clk_get_rate(info->clk);
		mys3c2416fb_activate_var(fbinfo);
	}

	return 0;
}
static inline int mys3c2416fb_cpufreq_register(struct mys3c2416fb_info *info)
{
	info->freq_transition.notifier_call = mys3c2416fb_cpufreq_transition;

	return cpufreq_register_notifier(&info->freq_transition, CPUFREQ_TRANSITION_NOTIFIER);
}

static inline void mys3c2416fb_cpufreq_deregister(struct mys3c2416fb_info *info)
{
	cpufreq_unregister_notifier(&info->freq_transition, CPUFREQ_TRANSITION_NOTIFIER);
}
#else
static inline int mys3c2416fb_cpufreq_register(struct mys3c2416fb_info *info)
{
	return 0;
}

static inline void mys3c2416fb_cpufreq_deregister(struct mys3c2416fb_info *info)
{
}
#endif


static const char driver_name[] = "mys3c2416-lcd";


static int mys3c2416fb_probe(struct platform_device *pdev)
{
	struct mys3c2416fb_info *info;
	struct mys3c2416fb_display *display;
	struct mys3c2416fb_mach_info *mach_info;

	struct fb_info *fbinfo;
	struct resource *res;
	int ret = 0;
	int irq;
	int i;
	int size;
	
	u32 vidcon0;


#if 0
	volatile unsigned long *virt_c   = NULL;
	volatile unsigned long *GPCCON = NULL;
	volatile unsigned long *GPCDAT = NULL;
	volatile unsigned long *GPCUP  = NULL;

	volatile unsigned long tem_dat = 0;

	virt_c = (volatile unsigned long *)ioremap(0x56000020,0x10);
	if(virt_c == NULL){
		printk(KERN_INFO "ioremap error \n");
		return -1;
	}
	GPCCON = (virt_c + 0);
	GPCDAT = (virt_c + 1);
	GPCUP  = (virt_c + 2);

	tem_dat = readl(GPCCON);
	tem_dat &= ~(3 << 0);
	tem_dat |= (1 << 0);
	writel(tem_dat,GPCCON);

	tem_dat = readl(GPCUP);
	tem_dat &= ~(3 << 0);
	tem_dat |= (2 << 0);
	writel(tem_dat,GPCUP);

	tem_dat = readl(GPCDAT);
	tem_dat |= (1 << 0);
	writel(tem_dat,GPCDAT);



//	writel(~(0x3 << 0),GPCCON);
//	writel( (0x1 << 0),GPCCON);
//	writel(~(0x3 << 0),GPCUP);
//	writel( (0x2 << 0),GPCUP);
//	writel( (0x1 << 0),GPCDAT);

	printk(KERN_INFO "GPCCON = %#x \n",*GPCCON);
#endif


	mach_info = dev_get_platdata(&pdev->dev);
	if(mach_info == NULL){
		printk(KERN_INFO "no platform data for lcd ,cannot attach \n");
		return -EINVAL;
	}
	printk(KERN_INFO "mach_info->displays->name = %s \n",mach_info->displays->name);

	if(mach_info->default_display >= mach_info->num_displays){
		printk(KERN_INFO "default is %d but only %d displays \n",
				mach_info->default_display, mach_info->num_displays);
		return -EINVAL;
	}

	display = mach_info->displays + mach_info->default_display;
	printk(KERN_INFO "display->name = %s \n",display->name);


	irq = platform_get_irq(pdev, 0);
	if(irq < 0){
		printk(KERN_INFO "no irq for device \n");
		return -EINVAL;
	}
	printk(KERN_INFO "s3c2416 used irq = %d \n",irq);

	/*分配 LCD 设备的私有数据*/
	fbinfo = framebuffer_alloc(sizeof(struct mys3c2416fb_info),&pdev->dev);
	if(!fbinfo){
		return -ENOMEM;
	}
	printk(KERN_INFO "fbinfo.fix.smem_start = %ld...... \n",fbinfo->fix.smem_start);
	printk(KERN_INFO "fbinfo.fix.smem_len = %ld...... \n",fbinfo->fix.smem_len);
	printk(KERN_INFO "fbinfo->screen_base = %#x...... \n",fbinfo->screen_base);

	platform_set_drvdata(pdev, fbinfo);

	info = fbinfo->par;
	info->dev = &pdev->dev;
//	info->drv_type = 0;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(res == NULL){
		printk(KERN_INFO "failed to get memory registers \n");
		ret = -ENXIO;
		goto dealloc_fb;
	}

	size = resource_size(res);

	printk(KERN_INFO "%s : %d size = %#x \n",__func__, __LINE__,size);
	printk(KERN_INFO "%s : %d res->start = %#x \n",__func__, __LINE__,res->start);
	printk(KERN_INFO "%s : %d pdev->name = %s \n",__func__, __LINE__,pdev->name);
	info->mem = request_mem_region(res->start, size, pdev->name);
	
	if(info->mem == NULL){
		printk(KERN_INFO  "failed to get memory region \n");
		goto dealloc_fb;
	}
    /* info->io 就是LCD寄存器的虚拟 首地址 */
	info->io = ioremap(res->start, size);
	if(info->io == NULL){
		printk(KERN_INFO "ioremap() of registers failed \n ");
		ret = -ENXIO;
		goto release_mem;
	}
	printk(KERN_INFO "%s : %d info->io = %p \n",__func__, __LINE__,info->io);

	info->irq_base = info->io + S3C2416_LCDINTBASE;
	printk(KERN_INFO "%s : %d info->irq_base = %#x \n",__func__,__LINE__,info->irq_base);

	printk(KERN_INFO "s3c2416 lcd devinit \n");

	strcpy(fbinfo->fix.id, driver_name);

	/*Stop the video*/
	vidcon0 = readl(info->io + S3C2416_VIDCON0);
	printk("vidcon0 .....[1] = %#x \n",vidcon0);
	vidcon0 &= ~(0x3 << 0);
	writel(vidcon0,info->io + S3C2416_VIDCON0);
	printk("vidcon0 .....[2] = %#x \n",vidcon0);

/*fix params */
	fbinfo->fix.type	    	= FB_TYPE_PACKED_PIXELS;
	fbinfo->fix.type_aux	    = 0;
	fbinfo->fix.xpanstep	    = 0;
	fbinfo->fix.ypanstep	    = 0;
	fbinfo->fix.ywrapstep	    = 0;
	fbinfo->fix.accel	    	= FB_ACCEL_NONE;
/*var params*/
	fbinfo->var.nonstd	    	= 0;
	fbinfo->var.activate	    = FB_ACTIVATE_NOW;
	fbinfo->var.accel_flags     = 0;
	fbinfo->var.vmode	    	= FB_VMODE_NONINTERLACED;
/*fbops*/
	fbinfo->fbops 				= &mys3c2416fb_ops;
	fbinfo->flags 				= FBINFO_FLAG_DEFAULT;
	fbinfo->pseudo_palette 		= &info->pseudo_pal;


	printk(KERN_INFO "###########################\n");


	for (i = 0; i < 256; i++)
		info->palette_buffer[i] = PALETTE_BUFF_CLEAR;

	ret = request_irq(irq, mys3c2416fb_irq, 0, pdev->name, info);
	if (ret) {
		printk(KERN_INFO "cannot get irq %d - err %d\n", irq, ret);
		ret = -EBUSY;
		goto release_regs;
	}
	
	printk(KERN_INFO "can get irq %d - ret  = %d\n", irq, ret);
	

	info->clk = clk_get(NULL, "lcd");
	if (IS_ERR(info->clk)) {
		printk(KERN_INFO "failed to get lcd clock source\n");
		ret = PTR_ERR(info->clk);
		goto release_irq;
	}

	printk(KERN_INFO "can  get lcd clock source\n");

	clk_prepare_enable(info->clk);
	printk("got and enabled clock\n");
	usleep_range(1000, 1100);

	info->clk_rate = clk_get_rate(info->clk);
	printk("%s : %d info->clk_rate = %ld\n",__func__,__LINE__,info->clk_rate);

	/* find maximum required memory size for display */
	for (i = 0; i < mach_info->num_displays; i++) {
		unsigned long smem_len = mach_info->displays[i].xres;

		smem_len *= mach_info->displays[i].yres;
		smem_len *= mach_info->displays[i].bpp;
		smem_len >>= 3;
		if (fbinfo->fix.smem_len < smem_len)
			fbinfo->fix.smem_len = smem_len;

		printk("%s : %d smem_len = %ld\n",__func__,__LINE__,smem_len);
	}
	printk("%s : %d mach_info->num_displays = %ld\n",__func__,__LINE__,mach_info->num_displays);



	/* Initialize video memory */
	ret = mys3c2416fb_map_video_memory(fbinfo);
	if (ret) {
		printk(KERN_INFO "Failed to allocate video RAM: %d\n", ret);
		ret = -ENOMEM;
		goto release_clock;
	}


	printk(KERN_INFO "%s : %d got video memory ret = %d \n",__func__,__LINE__,ret);


	fbinfo->var.xres = display->xres;
	fbinfo->var.yres = display->yres;
	fbinfo->var.bits_per_pixel = display->bpp;


	mys3c2416fb_init_registers(fbinfo);

	mys3c2416fb_check_var(&fbinfo->var, fbinfo);
	ret = mys3c2416fb_cpufreq_register(info);
	if (ret < 0) {
		printk(KERN_INFO "Failed to register cpufreq\n");
		goto free_video_memory;
	}
	printk(KERN_INFO "%s : %d mys3c2416fb_cpufreq_register\n",__func__,__LINE__);


	ret = register_framebuffer(fbinfo);
	if (ret < 0) {
		printk(KERN_INFO "Failed to register framebuffer device: %d\n",ret);
		goto free_cpufreq;
	}

	printk(KERN_INFO "%s : %d register_framebuffer\n",__func__,__LINE__);
	
	/* create device files*/
	ret = device_create_file(&pdev->dev, &dev_attr_debug);
	if (ret){
		printk(KERN_INFO "failed to add debug attribute\n");
	}

	printk(KERN_INFO "fb%d: %s frame buffer device\n", fbinfo->node, fbinfo->fix.id);
	mys3c2416fb_lcd_enable(info, 1);


	return 0;

free_cpufreq:
	mys3c2416fb_cpufreq_deregister(info);
free_video_memory:
	mys3c2416fb_unmap_video_memory(fbinfo);
release_clock:
	clk_disable_unprepare(info->clk);
	clk_put(info->clk);
release_irq:
	free_irq(irq, info);
release_regs:
	iounmap(info->io);
release_mem:
	release_mem_region(res->start, size);
dealloc_fb:
	framebuffer_release(fbinfo);

	return ret;
}


/*
 *Cleanup
 * */
static int mys3c2416fb_remove(struct platform_device *pdev)
{
	struct fb_info *fbinfo = platform_get_drvdata(pdev);
	struct mys3c2416fb_info *info = fbinfo->par;
	int irq;


	unregister_framebuffer(fbinfo);
	mys3c2416fb_cpufreq_deregister(info);
	mys3c2416fb_lcd_enable(info, 0);
	usleep_range(1000, 1100);

	mys3c2416fb_unmap_video_memory(fbinfo);

	if (info->clk) {
		clk_disable_unprepare(info->clk);
		clk_put(info->clk);
		info->clk = NULL;
	}

	irq = platform_get_irq(pdev, 0);
	free_irq(irq, info);

	iounmap(info->io);

	release_mem_region(info->mem->start, resource_size(info->mem));

	framebuffer_release(fbinfo);

	return 0;
}


static struct platform_driver mys3c2416fb_driver = {
	.probe    = mys3c2416fb_probe, 
	.remove   = mys3c2416fb_remove,
	.suspend  = mys3c2416fb_suspend,
	.resume   = mys3c2416fb_resume,
	.driver   = {
		.name = "mys3c2416-lcd",
		.owner = THIS_MODULE,
	},
};

#if 1
module_platform_driver(mys3c2416fb_driver);



#else
int __init mys3c2416fb_init(void)
{
	int ret = platform_driver_register(&mys3c2416fb_driver);
	if(ret == 0){
		printk(KERN_INFO "platform_driver_register lcd error!!\n");
	}else{
		printk(KERN_INFO "platform_driver_register lcd sucess!!\n");
	}
	return ret;
}

static void  __exit mys3c2416fb_exit(void)
{
	platform_driver_unregister(&mys3c2416fb_driver);
}


module_init(mys3c2416fb_init);
module_exit(mys3c2416fb_exit);
#endif


MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuczloveai910@gmail.com");
MODULE_DESCRIPTION("Framebuffer driver for the s3c2416");
MODULE_ALIAS("platform : s3c2416-lcd");


