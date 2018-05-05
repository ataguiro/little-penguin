#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Taguirov <ataguiro@student.42.fr>");
MODULE_DESCRIPTION("Hello World module with misc char");

#define LOGIN "ataguiro"
#define LOGIN_LEN 8

static struct miscdevice my_dev;

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
	if (retval)
	{
		retval = -EFAULT;
		goto out;
	}
	retval = (!strncmp(buf, LOGIN, LOGIN_LEN)) ? LOGIN_LEN : -EINVAL;
out:
	return retval;
}

static ssize_t hello_read(struct file *f, char __user *s, size_t n, loff_t *o)
{
	char *buf = LOGIN;

	if (n < LOGIN_LEN || *o >= LOGIN_LEN)
	{
		n = n < LOGIN_LEN ? -EINVAL : 0;
		goto out;
	}
	if (n > LOGIN_LEN)
		n = LOGIN_LEN;
	if (copy_to_user(s, buf, n))
	{
		n = -EFAULT;
		goto out;
	}
	*o += n;
out:
	return n;
}

struct file_operations my_fops = {
	.read = hello_read,
	.write = hello_write
};

static int __init hello_init(void) {
	int ret;
	printk(KERN_INFO "Hello World !\n");
	my_dev.minor = MISC_DYNAMIC_MINOR;
	my_dev.name = "fourtytwo";
	my_dev.fops = &my_fops;
	ret = misc_register(&my_dev);
	return 0;
}

static void __exit hello_cleanup(void) {
	printk(KERN_INFO "Cleaning up module.\n");
	misc_deregister(&my_dev);
}

module_init(hello_init);
module_exit(hello_cleanup);
