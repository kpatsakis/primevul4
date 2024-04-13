void __perf_event_task_sched_out(struct task_struct *task,
				 struct task_struct *next)
{
	int ctxn;

	for_each_task_context_nr(ctxn)
		perf_event_context_sched_out(task, ctxn, next);

	/*
	 * if cgroup events exist on this CPU, then we need
	 * to check if we have to switch out PMU state.
	 * cgroup event are system-wide mode only
	 */
	if (atomic_read(&__get_cpu_var(perf_cgroup_events)))
		perf_cgroup_sched_out(task, next);
}
