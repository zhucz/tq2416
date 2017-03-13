#include <asm/io.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>


#define LED_DEV_DRIVER_NAME		"plat-leds"
#define DEVICE_NAME				"platform-leds"

static volatile unsigned long *gpbcon = NULL;
static volatile unsigned long *gpbdat = NULL;
static volatile unsigned long *gpbudp = NULL;


static struct resource my_plat_leds_resource[] = {
	[0] = {
		.start = 0x56000010,
		.end   = 0x56000010 + 12,
		.flags = IORESOURCE_MEM,
	},
};


static void my_plat_leds_release(struct device *dev)
{
	return;
}

static struct platform_device my_plat_leds_dev = {
	.name = LED_DEV_DRIVER_NAME,
	.id = -1,
	.num_resources = ARRAY_SIZE(my_plat_leds_resource),
	.resource = my_plat_leds_resource,
	.dev = {
		.release = my_plat_leds_release,
	},
};
/*----above device resource apply----*/

int my_plat_leds_open (struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "my_plat_leds_open function........ok \n");

	writel(~(0xf << 10),gpbcon);
	writel((0x5 << 10),gpbcon);
	writel(~(0xf << 10),gpbudp);
	writel((0xa << 10),gpbudp);

	writel((0x1 << 5),gpbdat);
	return 0;
}

ssize_t my_plat_leds_read (struct file *filp, char __user *buf, size_t len, loff_t *off)
{

	printk(KERN_INFO "my_plat_leds_read function ........ok\n");
	return 0;
}

ssize_t my_plat_leds_write (struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "my_plat_leds_write function ........ok\n");
	return 0;
}

int my_plat_leds_close (struct inode *inode, struct file *filp)
{

	printk(KERN_INFO "my_plat_leds_close function ........ok\n");
	return 0;
}

long my_plat_leds_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		case 0:
			writel(~(0x1 << 5),gpbdat);
//			printk(KERN_INFO"led ON\n");
			break;
		case 1:
			writel((0x1 << 5),gpbdat);
//			printk(KERN_INFO"led OFF\n");
			break;
		default:
			break;
	}
	return 0;
}

static struct file_operations my_plat_leds_fops = {
	.owner = THIS_MODULE,
	.open = my_plat_leds_open,
	.read = my_plat_leds_read,
	.write = my_plat_leds_write,
	.unlocked_ioctl = my_plat_leds_ioctl,
	.release = my_plat_leds_close,
};


static struct miscdevice my_plat_leds_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,//   /dev/DEVICE_NAME   
	.fops = &my_plat_leds_fops,
};


static int my_plat_leds_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *pIORESOURCE_MEM;
	pIORESOURCE_MEM = platform_get_resource(pdev,IORESOURCE_MEM,0);
	gpbcon = ioremap(pIORESOURCE_MEM->start,pIORESOURCE_MEM->end - pIORESOURCE_MEM->start);
	printk(KERN_INFO "start %x \n",pIORESOURCE_MEM->start);
	gpbdat = gpbcon + 1;
	gpbudp = gpbcon + 2;

	ret = misc_register(&my_plat_leds_miscdev);
	return ret;
}

static int my_plat_leds_remove(struct platform_device *pdev)
{
	iounmap(gpbcon);
	misc_deregister(&my_plat_leds_miscdev);

	printk(KERN_INFO "my_plat_leds remove !\n");
	return 0;
}


static struct platform_driver my_plat_leds_driver = {
	.probe = my_plat_leds_probe,
	.remove = my_plat_leds_remove,
	.driver = {
		.name = LED_DEV_DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};


static int __init my_plat_leds_init(void)
{
	platform_device_register(&my_plat_leds_dev);
	printk(KERN_INFO "s3c2416 leds platform device register ok !\n");
	platform_driver_register(&my_plat_leds_driver);
	printk(KERN_INFO "s3c2416 leds platform driver register ok !\n");
	return 0;
}

static void __exit my_plat_leds_exit(void)
{
	platform_driver_unregister(&my_plat_leds_driver);
	printk(KERN_INFO "s3c2416 leds platform driver remove ok !\n");
	platform_device_unregister(&my_plat_leds_dev);
	printk(KERN_INFO "s3c2416 leds platform device remove ok !\n");
}


module_init(my_plat_leds_init);
module_exit(my_plat_leds_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuchengzhi@gmail.com");
MODULE_DESCRIPTION("this is platform device driver");
