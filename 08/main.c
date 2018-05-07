#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

// Dont have a license, LOL

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs);
static char *tmp2;

static const struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

char str[PAGE_SIZE] = {0};
char *tmp;

static int __init myfd_init(void)
{
	int retval;

	tmp2 = NULL;
	retval = misc_register(&myfd_device);
	return retval;
}

static void __exit myfd_cleanup(void)
{
	if (tmp2)
		kfree(tmp2);
	tmp2 = NULL;
	misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
	ssize_t t, i;

	/***************
	 * Malloc like a boss
	 ***************/

	if (tmp2)
		kfree(tmp2);
	tmp2 = kmalloc(sizeof(char) * PAGE_SIZE * 2, GFP_KERNEL);
	tmp = tmp2;
	for (t = strlen(str) - 1, i = 0; t >= 0; t--, i++)
		tmp[i] = str[t];
	tmp[i] = 0x0;
	return simple_read_from_buffer(user, size, offs, tmp, i);
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs)
{
	ssize_t res;

	res = simple_write_to_buffer(str, size, offs, user, size);
	str[size] = 0x0;
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
