#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>
#include <linux/mount.h>
#include <linux/sched.h>
#include <linux/nsproxy.h>
#include <linux/proc_fs.h>
#include <linux/fs_struct.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Taguirov <ataguiro@student.42.fr>");
MODULE_DESCRIPTION("Hello World module");

static int __init hello_init(void) {
	struct path path;
	struct dentry *thedentry;
	struct dentry *curdentry;
	char tmp[PATH_MAX];

	printk(KERN_INFO "Hello World !\n");
	kern_path("/", LOOKUP_FOLLOW, &path);
	thedentry = path.dentry;
	list_for_each_entry(curdentry, &current->fs->root.mnt->mnt_root->d_subdirs, d_child)
	{
		if (curdentry->d_flags & DCACHE_MOUNTED)
			printk("%s is mounted", curdentry->d_name.name);
	}

	return 0;
}

static void __exit hello_cleanup(void) {
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
