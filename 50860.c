static int proc_pid_syscall(struct seq_file *m, struct pid_namespace *ns,
			    struct pid *pid, struct task_struct *task)
{
	long nr;
	unsigned long args[6], sp, pc;
	int res;

	res = lock_trace(task);
	if (res)
		return res;

	if (task_current_syscall(task, &nr, args, 6, &sp, &pc))
		seq_puts(m, "running\n");
	else if (nr < 0)
		seq_printf(m, "%ld 0x%lx 0x%lx\n", nr, sp, pc);
	else
		seq_printf(m,
		       "%ld 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx 0x%lx\n",
		       nr,
		       args[0], args[1], args[2], args[3], args[4], args[5],
		       sp, pc);
	unlock_trace(task);

	return 0;
}
