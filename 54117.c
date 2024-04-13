static void cpu_cgroup_attach(struct cgroup_subsys_state *css,
			      struct cgroup_taskset *tset)
{
	struct task_struct *task;

	cgroup_taskset_for_each(task, css, tset)
		sched_move_task(task);
}
