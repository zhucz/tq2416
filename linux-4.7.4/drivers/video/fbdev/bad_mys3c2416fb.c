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


unsigned long lcdbase_addr;

static int mys3c2416fb_suspend(struct platform_device *dev, pm_message_t state)
{
	return 0;
}


static int mys3c2416fb_resume(struct platform_device *dev)
{
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
	}else{
		fbi->regs.vidcon0 &= ~S3C2416_VIDCON0_ENVID;
	}

	writel(fbi->regs.vidcon0, fbi->io + S3C2416_VIDCON0);

	local_irq_restore(flags);
}




static int mys3c2416fb_debug_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", debug ? "on" : "off");
}

static int mys3c2416fb_debug_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
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



/*
 *	mys3c2416fb_check_var():
 *	Get the video params out of 'var'. If a value doesn't fit, round it up,
 *	if it's too big, return -EINVAL.
 *
 */
static int mys3c2416fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	return 0;
}

static int mys3c2416fb_set_par(struct fb_info *info)
{
	return 0;
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
	return 0;
}

static int mys3c2416fb_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{

	return 0;
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


static unsigned long calc_fbsize(struct fb_info *info)
{
	struct mys3c2416fb_info *fbi = info->par;
	struct mys3c2416fb_mach_info *mach_info = dev_get_platdata(fbi->dev);
	int line_length = 0;
	unsigned long size = 0;

	line_length = ((mach_info->displays->xres * mach_info->displays->bpp)/8);
	size = line_length * mach_info->displays->yres;

	printk("%s : %d size = %ld \n",__func__,__LINE__,size);
	return size;
}


/*
 * mys3c2416fb_init_registers - Initialise all LCD-related registers
 */
static int mys3c2416fb_init_registers(struct fb_info *info,struct platform_device *pdev)
{
	struct mys3c2416fb_info *fbi = info->par;
	struct mys3c2416fb_mach_info *mach_info = dev_get_platdata(fbi->dev);
	void __iomem *regs = fbi->io;


	unsigned long flags;
	unsigned long freq_lcdclk;
	unsigned long freq_Hclk;
	unsigned long fb_size;
	unsigned char nn;

	void *v_lcd_base;

	local_irq_save(flags);
	/*GPIO Initialise 配置为RGB */
	modify_gpio(S3C2416_GPCUP,  mach_info->gpcup,  mach_info->gpcup_mask);
	modify_gpio(S3C2416_GPCCON, mach_info->gpccon, mach_info->gpccon_mask);
	modify_gpio(S3C2416_GPDUP,  mach_info->gpdup,  mach_info->gpdup_mask);
	modify_gpio(S3C2416_GPDCON, mach_info->gpdcon, mach_info->gpdcon_mask);

	/*LCD register Initialise*/
	mys3c2416fb_lcd_enable(fbi,0);//关闭Video output and the LCD
	/*关闭窗口0  和 窗口1*/
	fbi->regs.wincon0 &= ~(1 << S3C2416_WINCONX_ENWIN_F);
	writel(fbi->regs.wincon0,regs + S3C2416_WINCON0);
	fbi->regs.wincon1 &= ~(1 << S3C2416_WINCONX_ENWIN_F);
	writel(fbi->regs.wincon1,regs + S3C2416_WINCON0);

	freq_lcdclk = S3CFB_PIXEL_CLOCK;
	freq_Hclk = 133;
	nn = (unsigned char)(freq_Hclk/freq_lcdclk)-1;
	
	printk("%s : %d fbi->regs.vidcon0 = %#x \n",__func__,__LINE__,fbi->regs.vidcon0);
	printk("%s : %d regs = %p \n",__func__,__LINE__,regs);

	fbi->regs.vidcon0 = (VIDCON0_S_RGB_IF)|(VIDCON0_S_RGB_PAR)|(VIDCON0_S_VCLK_GATING_OFF)| \
						(VIDCON0_S_CLKDIR_DIVIDED)|(VIDCON0_S_CLKSEL_HCLK)|(VIDCON0_CLKVAL_F(nn));
	writel(fbi->regs.vidcon0,regs + S3C2416_VIDCON0);


	fbi->regs.vidcon1 = (VIDCON1_S_HSYNC_INVERTED)|(VIDCON1_S_VSYNC_INVERTED);
	writel(fbi->regs.vidcon1,regs + S3C2416_VIDCON1);

	fbi->regs.vidtcon0 = VIDTCON0_VBPD(S3CFB_VBP - 1)|VIDTCON0_VFPD(S3CFB_VFP - 1)|VIDTCON0_VSPW(S3CFB_VSW - 1);
	writel(fbi->regs.vidtcon0,regs + S3C2416_VIDTCON0);

	fbi->regs.vidtcon1 = VIDTCON1_HBPD(S3CFB_HBP - 1)|VIDTCON1_HFPD(S3CFB_HFP - 1)|VIDTCON1_HSPW(S3CFB_HSW - 1);
	writel(fbi->regs.vidtcon1,regs + S3C2416_VIDTCON1);

	fbi->regs.vidtcon2 = VIDTCON2_LINEVAL(S3CFB_VRES - 1)|VIDTCON2_HOZVAL(S3CFB_HRES - 1);
	writel(fbi->regs.vidtcon2,regs + S3C2416_VIDTCON2);

	fbi->regs.wincon0 = WINCONx_BPPMODE_F_16BPP_565|WINCONx_HAWSWP_ENABLE;
	writel(fbi->regs.wincon0,regs + S3C2416_WINCON0);

	fbi->regs.wincon1 = WINCONx_BPPMODE_F_16BPP_565|WINCONx_HAWSWP_ENABLE;
	writel(fbi->regs.wincon1,regs + S3C2416_WINCON1);

	fbi->regs.vidosd0a = VIDOSDxA_OSD_LTX_F(0)|VIDOSDxA_OSD_LTY_F(0);
	writel(fbi->regs.vidosd0a,regs + S3C2416_VIDOSD0A);

	fbi->regs.vidosd0b = VIDOSDxB_OSD_RBX_F(S3CFB_HRES - 1)|VIDOSDxB_OSD_RBY_F(S3CFB_VRES - 1);
	writel(fbi->regs.vidosd0b,regs + S3C2416_VIDOSD0B);

	fbi->regs.vidosd1a = VIDOSDxA_OSD_LTX_F(0)|VIDOSDxA_OSD_LTY_F(0);
	writel(fbi->regs.vidosd1a,regs + S3C2416_VIDOSD1A);

	fbi->regs.vidosd1b = VIDOSDxB_OSD_RBX_F(S3CFB_HRES - 1)|VIDOSDxB_OSD_RBY_F(S3CFB_VRES - 1);
	writel(fbi->regs.vidosd1b,regs + S3C2416_VIDOSD1B);

	fbi->regs.vidosd1c = 0xDDD000;
	writel(fbi->regs.vidosd1c,regs + S3C2416_VIDOSD1C);


	fb_size = calc_fbsize(info);//计算LCD的大小

	v_lcd_base = ioremap(lcdbase_addr, fb_size);
	if(v_lcd_base == NULL){
		dev_err(&pdev->dev, "ioremap() of registers lcdbase failed \n ");
		goto iounmapp;
	}

	writel(v_lcd_base,regs + S3C2416_VIDW00ADD0B0);
	writel(v_lcd_base + fb_size,regs + S3C2416_VIDW00ADD1B0);

	fbi->regs.vidw00add2b0 = VIDWxADDR2_OFFSIZE_F(0) | VIDWxADDR2_PAGEWIDTH_F(S3CFB_HRES * 2 );
	writel(fbi->regs.vidw00add2b0,regs + S3C2416_VIDW00ADD2B0);


	fbi->regs.w1keycon0 = WxKEYCON0_KEYBLEN_ENABLE|WxKEYCON0_KEYEN_F_ENABLE|WxKEYCON0_COMPKEY(0xffff);
	fbi->regs.w1keycon1 = 0x00000000;
	writel(fbi->regs.w1keycon0,regs + S3C2416_W1KEYCON0);
	writel(fbi->regs.w1keycon1,regs + S3C2416_W1KEYCON1);

	mys3c2416fb_lcd_enable(fbi,1);

	fbi->regs.wincon0 |= WINCONx_ENWIN_F_ENABLE;
	fbi->regs.wincon1 |= WINCONx_ENWIN_F_ENABLE;

	writel(fbi->regs.wincon0,regs + S3C2416_WINCON0);
	writel(fbi->regs.wincon1,regs + S3C2416_WINCON1);

	local_irq_restore(flags);


iounmapp:
	iounmap(v_lcd_base);



	return 0;
}




static const char driver_name[] = "mys3c2416fb";
static int mys3c2416fb_probe(struct platform_device *pdev)
{
	struct mys3c2416fb_info *info;
//	struct mys3c2416fb_display *display;
//	struct mys3c2416fb_mach_info *mach_info;

	struct fb_info *fbinfo;
	struct resource *res;
	int ret;
//	int irq;
	int size;

	/*描述一个具体的LCD设备，分配结构体空间 */
	fbinfo = framebuffer_alloc(sizeof(struct mys3c2416fb_info),&pdev->dev);
	if(!fbinfo){
		printk(KERN_INFO "Framebuffer_alloc failed.....\n");
		return -ENOMEM;
	}


	platform_set_drvdata(pdev,fbinfo);
	
	info = fbinfo->par;
	info->dev = &pdev->dev;

	res = platform_get_resource(pdev,IORESOURCE_MEM,0);
	if(res == NULL){
		dev_err(&pdev->dev, "failed to get memory !\n");
		ret = -ENXIO;
		goto dealloc_fb;
	}
	size = resource_size(res);

	/*申请物理内存*/
	info->mem = request_mem_region(res->start,size,pdev->name);
	if(info->mem == NULL){
		dev_err(&pdev->dev, "failed to get memory region!\n");
		ret = -ENXIO;
		goto dealloc_fb;
	}
	/*将刚才申请的物理内存转换为内核虚拟地址空间*/
	info->io = ioremap(res->start, size);
	if(info->io == NULL){
		dev_err(&pdev->dev, "ioremap() of registers failed \n ");
		ret = - ENXIO;
		goto release_mem;
	}
	
	printk("%s : %d info->io = %p \n",__func__,__LINE__,info->io);
	
	strcpy(fbinfo->fix.id, driver_name);

	/*初始化LCD控制器 */
	mys3c2416fb_init_registers(fbinfo,pdev);
	/*注册fbinfo*/
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
//	fbinfo->pseudo_palette 		= &info->pseudo_pal;
	fbinfo->fbops = &mys3c2416fb_ops;    //填充fb_info结构体
	ret = register_framebuffer(fbinfo);  //相当于注册 file_operations
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register framebuffer device: %d\n", ret);
		return -ENXIO ;
	}

	/*自动创建设备文件*/
	ret = device_create_file(&pdev->dev, &dev_attr_debug);
	if (ret){
		dev_err(&pdev->dev, "failed to add debug attribute\n");
	}

	printk("%s : %d mys3c2416 lcd found \n",__func__,__LINE__);

	return 0;

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

	unregister_framebuffer(fbinfo);
	mys3c2416fb_lcd_enable(info, 0);

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
	},

};


int __init mys3c2416fb_init(void)
{
	int ret = platform_driver_register(&mys3c2416fb_driver);
	if(ret == 0){
		pr_debug("platform_driver_register lcd error!!\n");
	}else{
		pr_debug("platform_driver_register lcd sucess!!\n");
	}
	return ret;
}

static void  __exit mys3c2416fb_exit(void)
{
	platform_driver_unregister(&mys3c2416fb_driver);
}


module_init(mys3c2416fb_init);
module_exit(mys3c2416fb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuczloveai910@gmail.com");
MODULE_DESCRIPTION("Framebuffer driver for the s3c2416");
MODULE_ALIAS("platform : s3c2416-lcd");


