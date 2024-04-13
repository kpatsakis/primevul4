void __perf_event_task_sched_in(struct task_struct *prev,
				struct task_struct *task)
{
	struct perf_event_context *ctx;
	int ctxn;

	for_each_task_context_nr(ctxn) {
		ctx = task->perf_event_ctxp[ctxn];
		if (likely(!ctx))
			continue;

		perf_event_context_sched_in(ctx, task);
	}
	/*
	 * if cgroup events exist on this CPU, then we need
	 * to check if we have to switch in PMU state.
	 * cgroup event are system-wide mode only
	 */
	if (atomic_read(&__get_cpu_var(perf_cgroup_events)))
		perf_cgroup_sched_in(prev, task);

	/* check for system-wide branch_stack events */
	if (atomic_read(&__get_cpu_var(perf_branch_stack_events)))
		perf_branch_stack_sched_in(prev, task);
}
