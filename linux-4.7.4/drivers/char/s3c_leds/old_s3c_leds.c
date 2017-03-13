#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/delay.h>


//#define LED_MAGIC	'M'
//#define LED_ON	_IO(LED_MAGIC,0)
//#define LED_OFF	_IO(LED_MAGIC,1)


#define LEDS_NAME 		"s3c_leds"
#define LEDS_MAJOR		251
#define LEDS_MINOR  	0


#define GPBCON	0x56000010
#define GPBDAT	0x56000014
#define GPBUDP	0x56000018


unsigned int *led_gpbcon;
unsigned int *led_gpbdat;
unsigned int *led_gpbudp;

dev_t s3c_leds_num;
struct cdev s3c_leds_cdev;


int s3c_leds_open(struct inode *inode, struct file *filp)
{
//	MOD_INC_USE_COUNT;
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
			writel(~(0x1 << 5),led_gpbdat);
//	*GPBDAT &= ~(1 << 5);
			printk(KERN_INFO"led ON\n");
			break;
		case 1:
			writel((0x1 << 5),led_gpbdat);
//	*GPBDAT |= (1 << 5);
			printk(KERN_INFO"led OFF\n");
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
	cdev_init(&s3c_leds_cdev,&s3c_leds_ops);
	alloc_chrdev_region(&s3c_leds_num,0,1,LEDS_NAME);
	cdev_add(&s3c_leds_cdev,s3c_leds_num,1);

	led_gpbcon = ioremap(GPBCON,4);
	writel((0x5 << 10),led_gpbcon);

	led_gpbdat = ioremap(GPBDAT,4);
	writel((0x1 << 5),led_gpbdat);


	printk(KERN_INFO "s3c_leds_init is ok........... \n");
	return 0;
}

static void __exit s3c_leds_exit(void)
{
	iounmap(led_gpbcon);
	iounmap(led_gpbdat);
	cdev_del(&s3c_leds_cdev);
	unregister_chrdev_region(s3c_leds_num,1);
	printk(KERN_INFO "s3c_leds_exit is ok........... \n");
}


module_init(s3c_leds_init);
module_exit(s3c_leds_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuchengzhi@gmail.com");
MODULE_DESCRIPTION("This driver for running LED!");

