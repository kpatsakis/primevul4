static inline void perf_cgroup_sched_out(struct task_struct *task,
					 struct task_struct *next)
{
	struct perf_cgroup *cgrp1;
	struct perf_cgroup *cgrp2 = NULL;

	rcu_read_lock();
	/*
	 * we come here when we know perf_cgroup_events > 0
	 * we do not need to pass the ctx here because we know
	 * we are holding the rcu lock
	 */
	cgrp1 = perf_cgroup_from_task(task, NULL);
	cgrp2 = perf_cgroup_from_task(next, NULL);

	/*
	 * only schedule out current cgroup events if we know
	 * that we are switching to a different cgroup. Otherwise,
	 * do no touch the cgroup events.
	 */
	if (cgrp1 != cgrp2)
		perf_cgroup_switch(task, PERF_CGROUP_SWOUT);

	rcu_read_unlock();
}
