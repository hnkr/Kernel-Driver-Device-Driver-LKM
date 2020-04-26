#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR("HUNKAR CIPLAK");
MODULE_DESCRIPTION("SIMPLE MODULE");
MODULE_LICENSE("GPL");


static __init int sm_init(void)
{
    printk(KERN_ALERT "simple_module:%s\n", __FUNCTION__);    
    return 0;
}

static __exit void sm_exit(void)
{
    printk(KERN_ALERT "simple_module:%s\n", __FUNCTION__);
}

module_init(sm_init);
module_exit(sm_exit);