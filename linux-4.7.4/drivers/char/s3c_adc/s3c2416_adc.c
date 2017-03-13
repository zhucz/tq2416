#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <plat/adc.h>
#include <plat/regs-adc.h>
#include <asm/uaccess.h>

int adc_major = 250;
int adc_minor = 0;
int number_of_device = 1;
dev_t devno;
struct cdev cdev;

struct s3c_adc_client *client;


static struct class *adc_class;


static int s3c2416_adc_open (struct inode *inode, struct file *filp)
{
	return 0;
}


static ssize_t s3c2416_adc_read (struct file *filp, char __user *buf, size_t len, loff_t *offset)
{
	unsigned int data;
	unsigned int ch;
	data = 10;
	ch = 2;
	data = s3c_adc_read(client, ch);
	printk(KERN_INFO "data = %d \n", data);
	if(copy_to_user(buf, (char *)&data, sizeof(data))){
		return -EFAULT;
	}
	return 0 ;
}

static int s3c2416_adc_release (struct inode *inode, struct file *filp)
{
	return 0 ;
}

static struct file_operations s3c2416_adc_fops = {
	.owner = THIS_MODULE,
	.open = s3c2416_adc_open,
	.read = s3c2416_adc_read,
	.release = s3c2416_adc_release,
};

static int s3c2416_adc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	int ret = -EINVAL;
	printk(KERN_INFO "function : %s \n",__func__);
	devno = MKDEV(adc_major,adc_minor);
	ret = register_chrdev_region(devno, number_of_device, "s3c2416_adc");
	if(ret){
	//	printk(KERN_INFO "register_chrdev_region failed \n");
		dev_err(dev, "register_chrdev_region failed \n");
		unregister_chrdev_region(devno, number_of_device);
		return ret;
	}

	cdev_init(&cdev, &s3c2416_adc_fops);
	cdev.owner = THIS_MODULE;
	ret = cdev_add(&cdev, devno, number_of_device);
	if(ret){
//		printk(KERN_INFO "cdev_add failed \n");
		dev_err(dev, "cdev_add failed \n");
		unregister_chrdev_region(devno, number_of_device);
		return ret;
	}


	adc_class = class_create(THIS_MODULE, "adc_class");
	if(IS_ERR(adc_class)){
		printk("Err: failed in creating class \n");
		return -1;
	}
	device_create(adc_class, NULL, devno, NULL, "adc");


	client = s3c_adc_register(pdev, NULL, NULL, 0);
	if(IS_ERR(client)){
//		printk(KERN_INFO "s3c_adc_register failed \n");
		dev_err(dev, "s3c_adc_register failed \n");
		cdev_del(&cdev);
		return ret;
	}

	dev_err(dev, "s3c2416_adc_probe ok \n");
	return 0;
}


static int s3c2416_adc_remove(struct platform_device *pdev)
{
	s3c_adc_release(client);
	cdev_del(&cdev);
	device_destroy(adc_class, devno);
	class_destroy(adc_class);
	unregister_chrdev_region(devno, number_of_device);
	printk("adc_class exit \n");
	return 0;
}



static struct platform_driver s3c2416_adc_driver = {
	.probe  = s3c2416_adc_probe,
	.remove = s3c2416_adc_remove,
	.driver = {
		.name = "s3c2416_adc",
		.owner = THIS_MODULE,
	},
};


static int __init s3c2416_adc_init(void)
{
	return	platform_driver_register(&s3c2416_adc_driver);
}

static void __exit s3c2416_adc_exit(void)
{
	platform_driver_unregister(&s3c2416_adc_driver);

}

module_init(s3c2416_adc_init);
module_exit(s3c2416_adc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuchengzhi@gmail.com ---- 2016-10-21-09:28:39");
MODULE_DESCRIPTION("soc arm926ejs s3c2416 adc module");
