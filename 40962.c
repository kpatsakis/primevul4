static void perf_cgroup_attach(struct cgroup *cgrp, struct cgroup_taskset *tset)
{
	struct task_struct *task;

	cgroup_taskset_for_each(task, cgrp, tset)
		task_function_call(task, __perf_cgroup_move, task);
}
