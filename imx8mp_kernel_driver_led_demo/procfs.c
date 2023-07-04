#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/virtio.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_open, single_release */
#include <uapi/linux/stat.h> /* S_IRUSR */
#include "rpmsg.h"
#include "procfs.h"
#include "commands.h"

static struct proc_dir_entry *procfs_dir;

static int proc_open_callback(struct inode *inode, struct file *file)
{
	return 0;
}

static int proc_release_callback(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t proc_read_callback(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
	int read = 0;
	char buffer[256];
	static int completed = 0;
	if (completed)
	{
		completed = 0;
		return 0;
	}

	completed = 1;
	read = sprintf(buffer, "leds %d %d %d %d\n", rpmsg_message.led_r, rpmsg_message.led_g, rpmsg_message.led_b,
			rpmsg_message.led_mode);
	if (copy_to_user(usr_buf, buffer, read))
	{
		return EFAULT;
	}

	return read;
}

static ssize_t proc_write_callback(struct file *file, const char __user *buffer, size_t count, loff_t *f_pos)
{
	char *tmp;
	if (count > PROCFS_MAX_SIZE)
	{
		return EFAULT;
	}

	tmp = kzalloc(count + 1, GFP_KERNEL);
	if (!tmp)
	{
		return -ENOMEM;
	}

	if (copy_from_user(tmp, buffer, count))
	{
		kfree(tmp);
		return EFAULT;
	}

	// pr_info("Wrote %lu bytes: %s\n", count, tmp);
	parse_line(tmp);

	kfree(tmp);
	return count;
}

static const struct proc_ops fops =
{
	.proc_open = proc_open_callback,
	.proc_read = proc_read_callback,
	.proc_write = proc_write_callback,
	.proc_release = proc_release_callback,
};

int procfs_init(void)
{
	procfs_dir = proc_create(PROCFS_NAME, 0, NULL, &fops);
	if (procfs_dir == NULL)
	{
		procfs_deinit();
		pr_info("PROCFS Error: Could not initialize /proc/%s.\n", PROCFS_NAME);
		return -ENOMEM;
	}

	pr_info("PROCFS /proc/%s created.\n", PROCFS_NAME);

	return 0;
}

void procfs_deinit(void)
{
	remove_proc_entry(PROCFS_NAME, NULL);
	pr_info("PROCFS /proc/%s removed.\n", PROCFS_NAME);
}
