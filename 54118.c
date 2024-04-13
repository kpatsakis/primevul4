static int cpu_cgroup_can_attach(struct cgroup_subsys_state *css,
				 struct cgroup_taskset *tset)
{
	struct task_struct *task;

	cgroup_taskset_for_each(task, css, tset) {
#ifdef CONFIG_RT_GROUP_SCHED
		if (!sched_rt_can_attach(css_tg(css), task))
			return -EINVAL;
#else
		/* We don't support RT-tasks being in separate groups */
		if (task->sched_class != &fair_sched_class)
			return -EINVAL;
#endif
	}
	return 0;
}
