static int proc_pid_wchan(struct seq_file *m, struct pid_namespace *ns,
			  struct pid *pid, struct task_struct *task)
{
	unsigned long wchan;
	char symname[KSYM_NAME_LEN];

	wchan = get_wchan(task);

	if (wchan && ptrace_may_access(task, PTRACE_MODE_READ_FSCREDS)
			&& !lookup_symbol_name(wchan, symname))
		seq_printf(m, "%s", symname);
	else
		seq_puts(m, "0\n");

	return 0;
}
