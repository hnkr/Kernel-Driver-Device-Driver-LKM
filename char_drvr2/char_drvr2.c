/*  char_drvr1.c
*   null driver which creates /dev file automatically.
*   Author:Hunkar Ciplak, hunkarciplak@hotmail.com, 04.2020
*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define CHAR_DEVICE_NAME    "char_drvr2"
#define CLASS_NAME          "char_drvr2_test"    //under /sys.
#define DEV_NAME            "char_drvr2_dev"    //under /dev

static dev_t device_num;
static struct class *dev_class;
static struct cdev char_dev;

MODULE_AUTHOR("HUNKAR CIPLAK");
MODULE_DESCRIPTION("CHAR_DRVR2");
MODULE_LICENSE("GPL");

DEFINE_MUTEX(mut_char_drvr2);


static int cd2_open(struct inode *, struct file *);
static int cd2_close(struct inode *, struct file *);
static ssize_t cd2_read(struct file *, char *buffer_, size_t , loff_t *);
static ssize_t cd2_write(struct file *, const char *, size_t , loff_t *);

static struct file_operations fs = {
    .owner = THIS_MODULE,
    .open = cd2_open,
    .read = cd2_read,
    .write = cd2_write,
    .release = cd2_close
};


static int __init cd2_init(void)
{
    struct device *ret_device;
    int ret_val;
    printk(KERN_INFO "%s:%s\n", CHAR_DEVICE_NAME, __FUNCTION__);
    //major-minor number allocation for char device
    ret_val = alloc_chrdev_region(&device_num, 0, 1, CHAR_DEVICE_NAME);
    if(ret_val < 0){
        printk(KERN_INFO "%s:Allocation can not be done!\n", __FUNCTION__);
        return ret_val;
    }
    printk(KERN_INFO "%s:Major-%d, Minor-%d\n", CHAR_DEVICE_NAME, MAJOR(device_num), MINOR(device_num));
    //class
    dev_class = class_create(fs.owner, CLASS_NAME);
    if(IS_ERR(dev_class)){
        printk(KERN_INFO "%s:Class can not be created\n", __FUNCTION__);
        unregister_chrdev_region(device_num, 1);
        return PTR_ERR(dev_class);
    }
    printk(KERN_INFO "%s:Class created under /sys:%s\n", __FUNCTION__, CLASS_NAME);
    //device
    ret_device = device_create(dev_class, NULL, device_num, NULL, DEV_NAME);
    if(IS_ERR(ret_device)){
        printk(KERN_INFO "%s:Device File can not be created\n", __FUNCTION__);
        class_destroy(dev_class);
        unregister_chrdev_region(device_num, 1);
        return PTR_ERR(ret_device);
    }
    
    printk(KERN_INFO "%s:Device file created under /dev:%s\n", __FUNCTION__, DEV_NAME);
    //char device
    cdev_init(&char_dev, &fs);
    ret_val = cdev_add(&char_dev, device_num, 1);
    if(ret_val < 0){
        device_destroy(dev_class, device_num);
        class_destroy(dev_class);
        unregister_chrdev_region(device_num, 1);
        return ret_val;
    }
    printk(KERN_INFO "%s:Char device added.\n", __FUNCTION__);
    mutex_init(&mut_char_drvr2);
    return 0;
}

static void __exit cd2_exit(void)
{
    printk(KERN_INFO "%s:%s\n", CHAR_DEVICE_NAME, __FUNCTION__);
    cdev_del(&char_dev);
    device_destroy(dev_class, device_num);
    class_destroy(dev_class);
    unregister_chrdev_region(device_num, 1);
    mutex_destroy(&mut_char_drvr2);
}

module_init(cd2_init);
module_exit(cd2_exit);

//io functions

static int cd2_open(struct inode *pinode_, struct file *pfile_)
{
    if(!mutex_trylock(&mut_char_drvr2)){
        printk(KERN_INFO "%s busy!\n", CHAR_DEVICE_NAME);
        return EBUSY;
    }
    printk(KERN_INFO "%s:%s\n", CHAR_DEVICE_NAME, __FUNCTION__);
    try_module_get(fs.owner);
    return 0;
}

static int cd2_close(struct inode *pinode, struct file *pfile)
{
    mutex_unlock(&mut_char_drvr2);
    module_put(fs.owner);
    printk(KERN_INFO "%s:%s\n", CHAR_DEVICE_NAME, __FUNCTION__);
    return 0;
}

static ssize_t cd2_read(struct file *pfile_, char *pbuffer_, size_t length_, loff_t *poffset_)
{
    printk(KERN_INFO "%s:%s\n", CHAR_DEVICE_NAME, __FUNCTION__);
    return length_;
}

static ssize_t cd2_write(struct file *pfile_, const char *pbuffer_, size_t length_, loff_t *poffset_)
{
    printk(KERN_INFO "%s:%s\n", CHAR_DEVICE_NAME, __FUNCTION__);
    return length_;
}


