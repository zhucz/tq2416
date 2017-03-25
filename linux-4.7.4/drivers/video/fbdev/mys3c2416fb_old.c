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

static unsigned long *mylcd_vidcon0;
static unsigned long *mylcd_vidcon1;
static unsigned long *mylcd_vidtcon0;
static unsigned long *mylcd_vidtcon1;
static unsigned long *mylcd_vidtcon2;
static unsigned long *mylcd_wincon0;
static unsigned long *mylcd_wincon1;
static unsigned long *mylcd_vidosd0a;
static unsigned long *mylcd_vidosd0b;

static unsigned long *mylcd_vidosd1a;
static unsigned long *mylcd_vidosd1b;
static unsigned long *mylcd_vidosd1c;

static unsigned long *mylcd_vidw00add0b0;
static unsigned long *mylcd_vidw00add0b1;

static unsigned long *mylcd_vidw01add0;

static unsigned long *mylcd_vidw00add1b0;
static unsigned long *mylcd_vidw00add1b1;

static unsigned long *mylcd_vidw01add1;

static unsigned long *mylcd_vidw00add2b0;
static unsigned long *mylcd_vidw00add2b1;

static unsigned long *mylcd_vidw01add2;




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
	mylcd_vidcon0 		= ioremap(0x4C800000,4);
	mylcd_vidcon1 		= ioremap(0x4C800004,4);
	mylcd_vidtcon0		= ioremap(0x4C800008,4);
	mylcd_vidtcon1		= ioremap(0x4C80000c,4);
	mylcd_vidtcon2		= ioremap(0x4C800010,4);
	mylcd_wincon0		= ioremap(0x4C800014,4);
	mylcd_wincon1		= ioremap(0x4C800018,4);
	mylcd_vidosd0a		= ioremap(0x4C800028,4);
	mylcd_vidosd0b		= ioremap(0x4C80002c,4);

	mylcd_vidosd1a		= ioremap(0x4C800034,4);
	mylcd_vidosd1b		= ioremap(0x4C800038,4);
	mylcd_vidosd1c		= ioremap(0x4C80003c,4);

	mylcd_vidw00add0b0	= ioremap(0x4C800064,4);
	mylcd_vidw00add0b1	= ioremap(0x4C800068,4);

	mylcd_vidw01add0	= ioremap(0x4C80006c,4);

	mylcd_vidw00add1b0	= ioremap(0x4C80007c,4);
	mylcd_vidw00add1b1	= ioremap(0x4C800080,4);

	mylcd_vidw01add1	= ioremap(0x4C800084,4);

	mylcd_vidw00add2b0	= ioremap(0x4C800094,4);
	mylcd_vidw00add2b1	= ioremap(0x4C800098,4);

	mylcd_vidw01add2	= ioremap(0x4C80009c,4);


	*mylcd_vidcon0 = (0<<0)|(0<<2)|(1<<4)|(1<<5)|(2<<6)|(0<<12)|(0<<13)|(0<<22);
	*mylcd_vidcon1 = (0<<4)|(1<<5)|(1<<6)|(0<<7)



	//跟具体CPU相关
	



	//设置通用的时间参数信息
	

//特定LCD寄存器的初始化



		//让内核帮你分配一个显存的




}


static void __exit mys3c2416_lcd_exit(void)
{


}

module_init(mys3c2416_lcd_init);
module_exit(mys3c2416_lcd_exit);

MODULE_LICENSE("GPL");

