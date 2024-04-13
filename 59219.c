static inline void perf_cgroup_sched_in(struct task_struct *prev,
					struct task_struct *task)
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
	cgrp2 = perf_cgroup_from_task(prev, NULL);

	/*
	 * only need to schedule in cgroup events if we are changing
	 * cgroup during ctxsw. Cgroup events were not scheduled
	 * out of ctxsw out if that was not the case.
	 */
	if (cgrp1 != cgrp2)
		perf_cgroup_switch(task, PERF_CGROUP_SWIN);

	rcu_read_unlock();
}
