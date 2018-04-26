#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
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
#include <linux/ns_common.h>
#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/poll.h>
#include <linux/ns_common.h>
#include <linux/fs_pin.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adam Taguirov <ataguiro@student.42.fr>");
MODULE_DESCRIPTION("Hello World module");

#define PROC_NAME "mymounts"

struct mnt_namespace {
        atomic_t                count;
        struct ns_common        ns;
        struct mount *  root;
        struct list_head        list;
        struct user_namespace   *user_ns;
        struct ucounts          *ucounts;
        u64                     seq;    /* Sequence number to prevent loops */
        wait_queue_head_t poll;
        u64 event;
        unsigned int            mounts; /* # of mounts in the namespace */
        unsigned int            pending_mounts;
} __randomize_layout;

struct mount {
        struct hlist_node mnt_hash;
        struct mount *mnt_parent;
        struct dentry *mnt_mountpoint;
        struct vfsmount mnt;
        union {
                struct rcu_head mnt_rcu;
                struct llist_node mnt_llist;
        };
#ifdef CONFIG_SMP
        struct mnt_pcp __percpu *mnt_pcp;
#else
        int mnt_count;
        int mnt_writers;
#endif
        struct list_head mnt_mounts;    /* list of children, anchored here */
        struct list_head mnt_child;     /* and going through their mnt_child */
        struct list_head mnt_instance;  /* mount instance on sb->s_mounts */
        const char *mnt_devname;        /* Name of device e.g. /dev/dsk/hda1 */
        struct list_head mnt_list;
        struct list_head mnt_expire;    /* link in fs-specific expiry list */
        struct list_head mnt_share;     /* circular list of shared mounts */
        struct list_head mnt_slave_list;/* list of slave mounts */
        struct list_head mnt_slave;     /* slave list entry */
        struct mount *mnt_master;       /* slave is on master->mnt_slave_list */
        struct mnt_namespace *mnt_ns;   /* containing namespace */
        struct mountpoint *mnt_mp;      /* where is it mounted */
        struct hlist_node mnt_mp_list;  /* list mounts with the same mountpoint */
        struct list_head mnt_umounting; /* list entry for umount propagation */
#ifdef CONFIG_FSNOTIFY
        struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
        __u32 mnt_fsnotify_mask;
#endif
        int mnt_id;                     /* mount identifier */
        int mnt_group_id;               /* peer group identifier */
        int mnt_expiry_mark;            /* true if marked for expiry */
        struct hlist_head mnt_pins;
        struct fs_pin mnt_umount;
        struct dentry *mnt_ex_mountpoint;
} __randomize_layout;

static char buf[256] = {0};
static char buffer_main[256] = {0};

static void fill_path(struct mount *parent)
{
	struct dentry *mnt_root = NULL;
	char *path;
	char buffer[256] = {0};

	memset(buffer, 0, 256);
	mnt_root = parent->mnt_mountpoint;
	path = dentry_path_raw(mnt_root, buffer, sizeof(buffer));
	if (parent->mnt_parent->mnt_id != parent->mnt_id  && parent->mnt_parent->mnt_id)
	{
		fill_path(parent->mnt_parent);
		strcat(buf, path);
	}
}

static int long_read(struct seq_file *m, void *v)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt_space;
	struct dentry *mnt_root;
	char *path = NULL;

	list_for_each_entry(mnt_space, &ns->list, mnt_list)
	{
		if (mnt_space->mnt_id)
		{
			memset(buf, 0, 256);
			mnt_root = mnt_space->mnt_mountpoint;
			memset(buffer_main, 0, 256);
			path = dentry_path_raw(mnt_root, buffer_main, sizeof(buffer_main));
			if (mnt_space->mnt_parent->mnt_id != mnt_space->mnt_id && mnt_space->mnt_parent->mnt_id)
				fill_path(mnt_space->mnt_parent);
			strcat(buf, path);
			seq_printf(m, "%s %s\n", mnt_space->mnt_devname, buf);
		}
	}
	return 0;
}

static int long_open(struct inode *inode, struct file *f)
{
	return single_open(f, long_read, NULL);
}

struct file_operations my_mounts = {
	.open = long_open,
	.read = seq_read
};

static int __init hello_init(void) {
	proc_create(PROC_NAME, 0, NULL, &my_mounts);
	return 0;
}

static void __exit hello_cleanup(void) {
	printk(KERN_INFO "Cleaning up module.\n");
	remove_proc_entry(PROC_NAME, NULL);
}

module_init(hello_init);
module_exit(hello_cleanup);
