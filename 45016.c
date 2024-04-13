static int proc_pid_stack(struct seq_file *m, struct pid_namespace *ns,
			  struct pid *pid, struct task_struct *task)
{
	struct stack_trace trace;
	unsigned long *entries;
	int i;

	entries = kmalloc(MAX_STACK_TRACE_DEPTH * sizeof(*entries), GFP_KERNEL);
	if (!entries)
		return -ENOMEM;

	trace.nr_entries	= 0;
	trace.max_entries	= MAX_STACK_TRACE_DEPTH;
	trace.entries		= entries;
	trace.skip		= 0;
	save_stack_trace_tsk(task, &trace);

	for (i = 0; i < trace.nr_entries; i++) {
		seq_printf(m, "[<%p>] %pS\n",
			   (void *)entries[i], (void *)entries[i]);
	}
	kfree(entries);

	return 0;
}
