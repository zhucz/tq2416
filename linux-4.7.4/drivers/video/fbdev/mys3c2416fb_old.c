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


#define MHZ				(1000*1000)
#define PRINT_MHZ(m)	((m) / MHZ),((m / 1000) % 1000)


/* 0x4C800000 ----  0x4C800BFC
 * 0xBFC = 3069字节 3072
 * */
//#define MYLCD_VIDCON0	((volatile unsigned long *)0x4C800000)

//#define MYLCD_GPBCON	((volatile unsigned long *)0x56000020)
//#define MYLCD_GPDCON	((volatile unsigned long *)0x56000030)

static unsigned long *mylcd_config_base;

static unsigned long *mylcd_gpbcon;
static unsigned long *mylcd_gpdcon;



static struct fb_info *mys3c2416_lcd;

/*提供通用的操作接口，当然你也可以提供自己私有的相关接口*/
static struct fb_ops mys3c2416_lcdfb_ops = {
	.owner = THIS_MODULE,
	.fb_fillrect  = cfb_fillrect,
	.fb_copyarea  = cfb_copyarea,
	.fb_imageblit = cfb_imageblit,
};


static int __init mys3c2416_lcd_init(void)
{
	struct clk *mys3c2416_clk;

	//分配fb_info
	mys3c2416_lcd = framebuffer_alloc(0,NULL);
	//初始化fb_info
	//2.1初始化LCD固定参数信息
	strcpy(mys3c2416_lcd->fix.id, "mys3c2416_lcd");
	mys3c2416_lcd->fix.smem_len = 480*272*2
	mys3c2416_lcd->fix.type = FB_TYPE_PACKED_PIXELS;
	mys3c2416_lcd->fix.visual = FB_VISUAL_TRUECOLOR;
	mys3c2416_lcd->fix.line_length = 480*2;
	//2.2初始化LCD可变参数信息
	mys3c2416_lcd->var.xres = 480;
	mys3c2416_lcd->var.yres = 272;

	mys3c2416_lcd->var.xres_virtual = 480;
	mys3c2416_lcd->var.yres_virtual = 272;
	
	mys3c2416_lcd->var.bits_per_pixel = 16;
	mys3c2416_lcd->var.red.offset   = 11; 
	mys3c2416_lcd->var.red.length   = 5;
	mys3c2416_lcd->var.green.length = 6;
	mys3c2416_lcd->var.green.length = 6;
	mys3c2416_lcd->var.blue.length  = 0;
	mys3c2416_lcd->var.blue.length  = 5;

	mys3c2416_lcd->var.activate = FB_ACTIVATE_NOW;


	//提供底层LCD操作结构，如果没有read,writ,ioctl,mmap,核心给你实现
	mys3c2416_lcd->fbops = &mys3c2416_lcdfb_ops;
	mys3c2416_lcd->screen_size = 480*272*2;

	//gpio的复用处理
	mylcd_gpbcon = ioremap(0x56000020,16);
	mylcd_gpdcon = ioremap(0x56000030,16);


	//内核为了实现电源管理，对于CPU的片上I2C，每一个的时钟都是独立的，
	//内核通过链表的形式来进行管理获取内核LCD时钟
	mys3c2416_clk = clk_get(NULL, "lcd");
	if(!mys3c2416_clk || IS_ERR(mys3c2416_clk)){
	
		printk(KERN_INFO "Failed to get lcd clock source \n");
	}
	clk_enable(mys3c2416_clk);


	//初始化LCD寄存器
	mylcd_config_base = ioremap(0x4C800000,3072)


}


static void __exit mys3c2416_lcd_exit(void)
{


}

module_init(mys3c2416_lcd_init);
module_exit(mys3c2416_lcd_exit);

MODULE_LICENSE("GPL");

