#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/mach-types.h>

#include <mach/hardware.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio-samsung.h>


#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>

#include <linux/delay.h>

#include <asm/io.h>

#define LEDS_NAME 		"s3c_leds"
#define LEDS_MAJOR		251
#define LEDS_MINOR  	0


volatile unsigned long *virt   = NULL;
volatile unsigned long *GPBCON = NULL;
volatile unsigned long *GPBDAT = NULL;
volatile unsigned long *GPBUP  = NULL;


volatile unsigned long *virt_e   = NULL;
volatile unsigned long *GPECON = NULL;
volatile unsigned long *GPEDAT = NULL;
volatile unsigned long *GPEUP  = NULL;


volatile unsigned long *virt_c   = NULL;
volatile unsigned long *GPCCON = NULL;
volatile unsigned long *GPCDAT = NULL;
volatile unsigned long *GPCUP  = NULL;


dev_t s3c_leds_num = 0;
struct cdev s3c_leds_cdev;


int s3c_leds_open(struct inode *inode, struct file *filp)
{
//	s3c_gpio_cfgpin(S3C2410_GPB(5),S3C2410_GPIO_OUTPUT);
//	writel((0x1 << 5),GPBDAT);
	printk(KERN_INFO "s3c_leds_open function........\n");
	return 0;
}

ssize_t s3c_leds_read (struct file *filp, char __user *buf, size_t len, loff_t *off)
{

	printk(KERN_INFO "s3c_leds_read function ........\n");
	return 0;
}

ssize_t s3c_leds_write (struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "s3c_leds_write function ........\n");
	return 0;
}

int s3c_leds_close (struct inode *inode, struct file *filp)
{

	printk(KERN_INFO "s3c_leds_write function ........\n");
	return 0;
}

long s3c_leds_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		case 0:
			writel(~(0x1 << 5),GPBDAT);
//			printk(KERN_INFO"led ON\n");
			break;
		case 1:
			writel((0x1 << 5),GPBDAT);
//			printk(KERN_INFO"led OFF\n");
			break;
		default:
			break;
	}
	return 0;
}


struct file_operations s3c_leds_ops = {
	.owner = THIS_MODULE,
	.open 			= s3c_leds_open,
	.read 			= s3c_leds_read,
	.write 			= s3c_leds_write,
	.unlocked_ioctl = s3c_leds_ioctl,
	.release		= s3c_leds_close,
};


static int __init s3c_leds_init(void)
{
	int ret = 0;
	
	s3c_leds_num = MKDEV(LEDS_MAJOR,LEDS_MINOR);
	ret = register_chrdev_region(s3c_leds_num,1,LEDS_NAME);
	if(ret < 0){
		printk(KERN_INFO "register char region error \n");
		return ret;
	}
	cdev_init(&s3c_leds_cdev,&s3c_leds_ops);
	s3c_leds_cdev.owner = THIS_MODULE;

	ret = cdev_add(&s3c_leds_cdev,s3c_leds_num,1);
	if(ret < 0){
		printk(KERN_INFO "cdev_add error \n");
		return ret;
	}

	virt = (volatile unsigned long *)ioremap(0x56000010,0x10);
	if(virt == NULL){
		printk(KERN_INFO "ioremap error \n");
		return -1;
	}
	GPBCON = (virt + 0);
	GPBDAT = (virt + 1);
	GPBUP  = (virt + 2);



	virt_e = (volatile unsigned long *)ioremap(0x56000040,0x10);
	if(virt_e == NULL){
		printk(KERN_INFO "ioremap error \n");
		return -1;
	}
	GPECON = (virt_e + 0);
	GPEDAT = (virt_e + 1);
	GPEUP  = (virt_e + 2);

	virt_c = (volatile unsigned long *)ioremap(0x56000020,0x10);
	if(virt_c == NULL){
		printk(KERN_INFO "ioremap error \n");
		return -1;
	}
	GPCCON = (virt_c + 0);
	GPCDAT = (virt_c + 1);
	GPCUP  = (virt_c + 2);


#if 0
	*GPBCON &= ~(0xf << 10);
	*GPBCON |=  (0x5 << 10);
	*GPBUP  &= ~(0xf << 10);
	*GPBUP  |=  (0xa << 10);

	*GPBDAT &= ~(1 << 5);
	mdelay(1000);
	*GPBDAT |= (1 << 5);
	mdelay(1000);
	*GPBDAT &= ~(1 << 5);

/*------------------------------------------*/
#else
	writel(~(0xf << 10),GPBCON);
	writel((0x5 << 10),GPBCON);
	writel(~(0xf << 10),GPBUP);
	writel((0xa << 10),GPBUP);

	writel((0x1 << 5),GPBDAT);


#if 1
//	printk(KERN_INFO "lcd display start........... \n");
	writel(~(0xf << 28),GPECON);
	writel( (0x5 << 28),GPECON);
	writel(~(0xf << 28),GPEUP);
	writel( (0xa << 28),GPEUP);
//	printk(KERN_INFO "lcd display  end........... \n");
#endif
#endif


	printk(KERN_INFO "s3c_leds_init is ok........... \n");
	return 0;
}

static void __exit s3c_leds_exit(void)
{
	cdev_del(&s3c_leds_cdev);
	iounmap((void *)virt);
	unregister_chrdev_region(s3c_leds_num,1);
	printk(KERN_INFO "s3c_leds_exit is ok........... \n");
}


module_init(s3c_leds_init);
module_exit(s3c_leds_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuchengzhi@gmail.com");
MODULE_DESCRIPTION("This driver for running LED!");

