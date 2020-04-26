/*  char_drvr1.c
*   null driver which requires manually created /dev file.
*   Author:Hunkar Ciplak, hunkarciplak@hotmail.com, 04.2020
*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mutex.h>


#define DEVICE_NAME     "char_drvr1"    //name in proc/devices

DEFINE_MUTEX 	(mut_char_drvr1);

MODULE_AUTHOR("HUNKAR CIPLAK");
MODULE_DESCRIPTION("CHAR_DRVR1");
MODULE_LICENSE("GPL");

static int major_number;

static int cd1_open(struct inode *, struct file *);
static int cd1_close(struct inode *, struct file *);
static ssize_t cd1_read(struct file *, char *, size_t, loff_t *);
static ssize_t cd1_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops = {
    .open = cd1_open,
    .read = cd1_read,
    .write = cd1_write,
    .release = cd1_close,
    .owner = THIS_MODULE
};


static __init int cd1_init(void)
{
    printk(KERN_INFO "%s:%s\n", DEVICE_NAME, __FUNCTION__);
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if(major_number < 0){
        printk(KERN_INFO "%s:%s registration failed", DEVICE_NAME, __FUNCTION__);
        return major_number;
    }
    printk(KERN_INFO "Registration done. Major number:%d\n", major_number);
    printk(KERN_INFO "In order to create dev file:sudo mknod -m 666 /dev/char_driver1_test c %d 0\n", major_number);
    mutex_init(&mut_char_drvr1);
    return 0;
}

static __exit void cd1_exit(void)
{
    printk(KERN_INFO "%s:%s\n", DEVICE_NAME, __FUNCTION__);
    unregister_chrdev(major_number, DEVICE_NAME);
    mutex_destroy(&mut_char_drvr1);
}

module_init(cd1_init);
module_exit(cd1_exit);

// File Operations..

static int cd1_open(struct inode *pinode_, struct file * pfile_)
{
   if(!mutex_trylock(&mut_char_drvr1)){
       printk(KERN_INFO "Module is in use!\n");
       return -EBUSY;
   }
    printk(KERN_INFO "%s:%s\n", DEVICE_NAME, __FUNCTION__);
    try_module_get(fops.owner);
    return 0;
}

static int cd1_close(struct inode *pinode_, struct file *pfile_)
{
    printk(KERN_INFO "%s:%s\n", DEVICE_NAME, __FUNCTION__);
    mutex_unlock(&mut_char_drvr1);
    module_put(fops.owner);
    return 0;
}

static ssize_t cd1_read(struct file *pfile_, char *pbuffer_, size_t length_, loff_t *poffset_)
{
    printk(KERN_INFO "%s:%s\n", DEVICE_NAME, __FUNCTION__);
    return length_;
}

static ssize_t cd1_write(struct file *pfile_, const char *pbuffer_, size_t length_, loff_t *poffset_)
{
    printk(KERN_INFO "%s:%s\n", DEVICE_NAME, __FUNCTION__);
    return length_;
}

