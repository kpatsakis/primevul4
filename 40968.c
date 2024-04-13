static inline void perf_cgroup_sched_out(struct task_struct *task,
					 struct task_struct *next)
{
	struct perf_cgroup *cgrp1;
	struct perf_cgroup *cgrp2 = NULL;

	/*
	 * we come here when we know perf_cgroup_events > 0
	 */
	cgrp1 = perf_cgroup_from_task(task);

	/*
	 * next is NULL when called from perf_event_enable_on_exec()
	 * that will systematically cause a cgroup_switch()
	 */
	if (next)
		cgrp2 = perf_cgroup_from_task(next);

	/*
	 * only schedule out current cgroup events if we know
	 * that we are switching to a different cgroup. Otherwise,
	 * do no touch the cgroup events.
	 */
	if (cgrp1 != cgrp2)
		perf_cgroup_switch(task, PERF_CGROUP_SWOUT);
}
