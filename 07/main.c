#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Taguirov <ataguiro@student.42.fr>");
MODULE_DESCRIPTION("Hello World module with debugfs structure");

#define LOGIN "ataguiro"
#define LOGIN_LEN 8

static struct dentry *my_root;
static struct mutex g_mutex;
static char page_buf[PAGE_SIZE] = {0};

static ssize_t id_write(struct file *f, const char __user *s, size_t n, loff_t *o)
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


static ssize_t id_read(struct file *f, char __user *s, size_t n, loff_t *o)
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

static ssize_t foo_write(struct file *f, const char __user *s, size_t n, loff_t *o)
{
	int ret = -EINVAL;

	ret = mutex_lock_interruptible(&g_mutex);
	if (ret)
		goto out;
	if (n > PAGE_SIZE)
	{
		ret = -EINVAL;
		goto out;
	}
	ret = copy_from_user(page_buf, s, n);
	page_buf[n] = 0;
	ret = ret ? -EFAULT : n;
out:
	mutex_unlock(&g_mutex);
	return ret;
}

static ssize_t foo_read(struct file *f, char __user *s, size_t n, loff_t *o)
{
	int len;

	len = mutex_lock_interruptible(&g_mutex);
	if (len)
	{
		n = len;
		goto out;
	}
	len = strlen(page_buf);
	if (*o >= len)
	{
		n = 0;
		goto out;
	}
	if (*o + n > len)
		n = len - *o;
	if (copy_to_user(s, page_buf + *o, n))
	{
		n = -EFAULT;
		goto out;
	}
	*o += n;
out:
	mutex_unlock(&g_mutex);
	return n;
}

struct file_operations my_fops_id = {
	.read = id_read,
	.write = id_write
};

struct file_operations my_fops_foo = {
	.read = foo_read,
	.write = foo_write
};

static int __init hello_init(void) {
	int retval = 0;

	printk(KERN_INFO "Hello World !\n");
	mutex_init(&g_mutex);
	my_root = debugfs_create_dir("fourtytwo", NULL);
	if (!my_root)
	{
		retval = -ENOENT;
		goto out;
	}
	if (!debugfs_create_file("id", 0666, my_root, NULL, &my_fops_id)
		|| !debugfs_create_file("foo", 0644, my_root, NULL, &my_fops_foo)
		|| !debugfs_create_u64("jiffies", 0444, my_root, (u64*)&jiffies))
		retval = -ENOENT;
out:
	if (!my_root)
		debugfs_remove_recursive(my_root);
	return retval;
}

static void __exit hello_cleanup(void) {
	printk(KERN_INFO "Cleaning up module.\n");
	debugfs_remove_recursive(my_root);
	mutex_destroy(&g_mutex);
}

module_init(hello_init);
module_exit(hello_cleanup);
