#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Taguirov <ataguiro@student.42.fr>");
MODULE_DESCRIPTION("Hello World module");

#define LOGIN "ataguiro"
#define LOGIN_LEN 8

static struct dentry *my_root;

static ssize_t hello_write(struct file *f, const char __user *s, size_t n, loff_t *o)
{
	char buf[LOGIN_LEN];
	int retval = -EINVAL;	

	if (n != LOGIN_LEN)
	{
		retval = -EINVAL;
		goto out;
	}
	retval = copy_from_user(buf, s, LOGIN_LEN);
	printk(KERN_INFO "Copied from user: [%s] of size %zu\n", buf, n);
	retval = (!strncmp(buf, LOGIN, LOGIN_LEN)) ? LOGIN_LEN : -EINVAL;
out:
	return retval;
}


static ssize_t hello_read(struct file *f, char __user *s, size_t n, loff_t *o)
{
	char *buf = LOGIN;

	if (*o >= LOGIN_LEN)
	{
		n = 0;
		goto out;
	}
	if (*o + n > LOGIN_LEN)
		n = LOGIN_LEN - *o;
	if (copy_to_user(s, buf + *o, n))
	{
		n = -EFAULT;
		goto out;
	}
	*o += n;
	printk(KERN_INFO "Copied to user: [%s] of size %zu\n", buf, n);
out:
	return n;
}


struct file_operations my_fops = {
	.read = hello_read,
	.write = hello_write
};

static int __init hello_init(void) {
	int retval = 0;

	printk(KERN_INFO "Hello World !\n");
	my_root = debugfs_create_dir("fourtytwo", NULL);
	if (!my_root)
	{
		retval = -ENOENT;
		goto out;
	}
	if (!debugfs_create_file("id", 0644, my_root, NULL, &my_fops))
		retval = -ENOENT;	

out:
	if (!my_root)
		debugfs_remove_recursive(my_root);
	return retval;
}

static void __exit hello_cleanup(void) {
	printk(KERN_INFO "Cleaning up module.\n");
	debugfs_remove_recursive(my_root);
}

module_init(hello_init);
module_exit(hello_cleanup);
