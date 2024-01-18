#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

static struct proc_dir_entry* my_new_entry;

static int proc_count(struct seq_file *s, void *v){
	struct task_struct* t;
	int procs_num = 0;
	for_each_process(t) {
		procs_num++;
	}
	seq_printf(s, "%d\n", procs_num);
	return 0;
	
}
static int __init proc_count_init(void)
{
	//pr_info("proc_count: init\n");
	my_new_entry = proc_create_single("count", 0, NULL, proc_count);

	return 0;
}

static void __exit proc_count_exit(void)
{
	//pr_info("proc_count: exit\n");
	proc_remove(my_new_entry);
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Jaelyn Fan");
MODULE_DESCRIPTION("Count the current number of running processes");
MODULE_LICENSE("GPL");
