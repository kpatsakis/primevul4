static int proc_tid_io_accounting(struct seq_file *m, struct pid_namespace *ns,
				  struct pid *pid, struct task_struct *task)
{
	return do_io_accounting(task, m, 0);
}
