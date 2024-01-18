#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/signal.h> // Necessary for iterating through the task list

static struct proc_dir_entry *entry;

// This function counts and prints the number of processes
static int proc_count(struct seq_file *m, void *v){
    struct task_struct *task;
    int process_count = 0;  // Variable to hold the number of processes

    // Iterate through all the processes and increment the count
    for_each_process(task) {
        process_count++;
    }

    // Write the number of processes to the seq_file
    seq_printf(m, "%d\n", process_count);
    return 0;
}

static int __init proc_count_init(void) {
    // Create the proc entry upon module initialization
    entry = proc_create_single("count", 0, NULL, proc_count);
    //pr_info("proc_count: Module loaded and /proc/count created\n");
    return 0;
}

static void __exit proc_count_exit(void) {
    // Remove the proc entry upon module cleanup
    proc_remove(entry);
    //pr_info("proc_count: Module unloaded and /proc/count removed\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Mingqi zhao");
MODULE_DESCRIPTION("Count the current number of running processes");
MODULE_LICENSE("GPL");