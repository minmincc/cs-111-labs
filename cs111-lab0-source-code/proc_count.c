#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

static struct proc_dir_entry *entry;

static int proc_count(struct seq_file *m, void *v){
    struct task_struct *task;
    int count = 0;

    for_each_process(task) {
        count++;
    }

    seq_printf(m, "%d\n", count);
    return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_count, NULL);
}

static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init proc_count_init(void)
{
    entry = proc_create("count", 0, NULL, &proc_fops);
    if (!entry) {
        return -ENOMEM;
    }

    pr_info("proc_count: init\n");
    return 0;
}

static void __exit proc_count_exit(void)
{
    proc_remove(entry);
    pr_info("proc_count: exit\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("CS111 lab0 count proc number");
MODULE_LICENSE("GPL");