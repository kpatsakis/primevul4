static int proc_pid_personality(struct seq_file *m, struct pid_namespace *ns,
				struct pid *pid, struct task_struct *task)
{
	seq_printf(m, "%08x\n", task->personality);
	return 0;
}
