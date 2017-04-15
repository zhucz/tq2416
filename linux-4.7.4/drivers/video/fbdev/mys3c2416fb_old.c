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




static unsigned long *mylcd_gpbcon;//10
static unsigned long *mylcd_gpbdat;//14

static unsigned long *mylcd_gpccon;//20
static unsigned long *mylcd_gpcdat;//24

static unsigned long *mylcd_gpdcon;//30
static unsigned long *mylcd_gpddat;//34

static struct fb_info *mys3c2416_lcd;

/*提供通用的操作接口，当然你也可以提供自己私有的相关接口*/
static struct fb_ops mys3c2416_lcdfb_ops = {
	.owner = THIS_MODULE,
	.fb_fillrect  = cfb_fillrect,
	.fb_copyarea  = cfb_copyarea,
	.fb_imageblit = cfb_imageblit,
};


void LCD_Enable(int Enable)
{
	if(Enable){
		*mylcd_vidcon0 |= (0x3 << 0);
	}else{
		*mylcd_vidcon0 &= ~(0x3 << 0);
	}
}


void LCD_BackLight(int On)
{
	*mylcd_gpbcon &= ~(0x3 << 0);
	*mylcd_gpbcon |=  (0x1 << 0);

	if(On){
		*mylcd_gpbdat |= (0x1 << 0);
	}else{
		*mylcd_gpbdat &= ~(0x1 << 0);
	}
}

static int __init mys3c2416_lcd_init(void)
{
	struct clk *mys3c2416_clk;
	unsigned long			clk_rate;


	//分配fb_info
	mys3c2416_lcd = framebuffer_alloc(0,NULL);
	//初始化fb_info
	//2.1初始化LCD固定参数信息
	strcpy(mys3c2416_lcd->fix.id, "mys3c2416_lcd");
	mys3c2416_lcd->fix.smem_len = 480*272*2;
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
	mylcd_gpbcon = ioremap(0x56000010,4);
	mylcd_gpbdat = ioremap(0x56000014,4);

	mylcd_gpccon = ioremap(0x56000020,4);
	mylcd_gpcdat = ioremap(0x56000024,4);

	mylcd_gpdcon = ioremap(0x56000030,4);
	mylcd_gpddat = ioremap(0x56000034,4);

	// 1010 1010 1010 1010 0000 0010 1010 1010   
	*mylcd_gpbcon = 0xAAAA02AA;
    // 1010 1010 1010 1010 1010 1010 1010 1010
	*mylcd_gpdcon = 0xAAAAAAAA;

	//内核为了实现电源管理，对于CPU的片上I2C，每一个的时钟都是独立的，
	//内核通过链表的形式来进行管理获取内核LCD时钟
	mys3c2416_clk = clk_get(NULL, "lcd");
	if(IS_ERR(mys3c2416_clk)){
	
		printk(KERN_INFO "Failed to get lcd clock source \n");
	}
//	clk_enable(mys3c2416_clk);
	clk_prepare_enable(mys3c2416_clk);
	printk("got and enableed clock \ n");
	usleep_range(1000, 1100);

	clk_rate = clk_get_rate(mys3c2416_clk);
	printk("%s : %d info->clk_rate = %ld\n",__func__,__LINE__,clk_rate);

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



	//跟具体CPU相关
	
	//选择HCLK=133M，3分频得到VCLK=33.3M RGB并口格式(RGB)，
	//暂不启动控制逻辑
	*mylcd_vidcon0 = (0<<0)|(0<<2)|(1<<4)|(1<<5)|(2<<6)|(0<<12)|(0<<13)|(0<<22);
	//VCLK下降沿锁存数据，行场同步信号低激活，数据使能高有效
	*mylcd_vidcon1 = (0<<4)|(1<<5)|(1<<6)|(0<<7);
	*mylcd_vidtcon0 = ((S3CFB_VBP-1)<<16)|((S3CFB_VFP-1)<<8)|((S3CFB_VSW-1)<<0);
	*mylcd_vidtcon1 = ((S3CFB_HBP-1)<<16)|((S3CFB_HFP-1)<<8)|((S3CFB_HSW-1)<<0);
    
	//设置屏幕像素尺寸
	*mylcd_vidtcon2 = ((S3CFB_VRES-1)<<11)|((S3CFB_HRES-1)<<0);

	//设置通用的时间参数信息
	//特定LCD寄存器的初始化
	//设置OSD图像与屏幕尺寸一致
	//
	*mylcd_vidosd0a = (0<<11)|(0<<0);
	*mylcd_vidosd0b = ((S3CFB_HRES-1)<<11)|((S3CFB_VRES-1)<<0);
	*mylcd_vidosd1a = (0<<11)|(0<<0);
	*mylcd_vidosd1b = ((S3CFB_HRES-1)<<11)|((S3CFB_VRES-1)<<0);

	//alpha混合方式，基色匹配时全透明，未匹配部分完全不透明
	*mylcd_vidosd1c = 0xfff000;


	//让内核帮你分配一个显存的起始物理地址，并且分配对应物理地址的
	//内核虚拟地址
	//起始物理地址：smem_start
	//起始内核虚拟地址：screen_base
	mys3c2416_lcd->screen_base = dma_alloc_writecombine(NULL, \
			mys3c2416_lcd->fix.smem_len,  					  \
			(dma_addr_t *)&mys3c2416_lcd->fix.smem_start,     \
			GFP_KERNEL);


	//通用的硬件初始化
	//告诉CPU显存的起始物理地址和起始结束地址
	//将这两个地址设置到LCD对应的寄存器中即可
	*mylcd_vidw00add0b0 = mys3c2416_lcd->fix.smem_start;
	*mylcd_vidw00add1b0 = mys3c2416_lcd->fix.smem_start + mys3c2416_lcd->fix.smem_len;


	//不使用虚拟屏幕
	*mylcd_vidw00add2b0 = (0<<13)|((S3CFB_HRES*2)<<0);
	//启动LCD控制其，等待用户访问操作显存
	*mylcd_wincon0 |= (1 << 0);

	LCD_Enable(1);
	LCD_BackLight(1);


	//向核心层注册fb_info
	ret = register_framebuffer(mys3c2416_lcd);
	if (ret < 0) {
		printk("Failed to register framebuffer device: %d\n",ret);
	}
	printk(KERN_INFO "%s : %d register_framebuffer\n",__func__,__LINE__);
	
	return 0;

}


static void __exit mys3c2416_lcd_exit(void)
{
	unregister_framebuffer(mys3c2416_lcd);

	if (mys3c2416_clk) {
		clk_disable_unprepare(mys3c2416_clk);
		clk_put(mys3c2416_clk);
		mys3c2416_clk = NULL;
	}


	dma_free_writecombine(NULL,               \
			mys3c2416_lcd->fix.smem_len,      \
			mys3c2416_lcd->screen_base,       \
			mys3c2416_lcd->fix.smem_start);

	iounmap(mylcd_gpbcon);
	iounmap(mylcd_gpdcon);

	iounmap(mylcd_vidcon0);
	iounmap(mylcd_vidcon1);	
	iounmap(mylcd_vidtcon0);		
	iounmap(mylcd_vidtcon1);	
	iounmap(mylcd_vidtcon2);	
	iounmap(mylcd_wincon0);		
	iounmap(mylcd_wincon1);	
	iounmap(mylcd_vidosd0a);		
	iounmap(mylcd_vidosd0b);	

	iounmap(mylcd_vidosd1a);		
	iounmap(mylcd_vidosd1b);		
	iounmap(mylcd_vidosd1c);	

	iounmap(mylcd_vidw00add0b0);	
	iounmap(mylcd_vidw00add0b1);	

	iounmap(mylcd_vidw01add0);	

	iounmap(mylcd_vidw00add1b0);
	iounmap(mylcd_vidw00add1b1);	

	iounmap(mylcd_vidw01add1);	

	iounmap(mylcd_vidw00add2b0);	
	iounmap(mylcd_vidw00add2b1);	

	iounmap(mylcd_vidw01add2);

	framebuffer_release(mys3c2416_lcd);
}

module_init(mys3c2416_lcd_init);
module_exit(mys3c2416_lcd_exit);

MODULE_LICENSE("GPL");

