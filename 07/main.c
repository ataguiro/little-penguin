#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Taguirov <ataguiro@student.42.fr>");
MODULE_DESCRIPTION("Hello World module");

static struct dentry *my_root;

static int __init hello_init(void) {
	int retval = 0;

	printk(KERN_INFO "Hello World !\n");
	my_root = debugfs_create_dir("fourtytwo", NULL);
	if (!my_root)
	{
		retval = -ENOENT;
		goto out;
	}

out:
	if (!my_root)
		debugfs_remove_recursive(&my_root);
	return retval;
}

static void __exit hello_cleanup(void) {
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
