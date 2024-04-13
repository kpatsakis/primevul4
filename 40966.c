static void perf_cgroup_exit(struct cgroup *cgrp, struct cgroup *old_cgrp,
			     struct task_struct *task)
{
	/*
	 * cgroup_exit() is called in the copy_process() failure path.
	 * Ignore this case since the task hasn't ran yet, this avoids
	 * trying to poke a half freed task state from generic code.
	 */
	if (!(task->flags & PF_EXITING))
		return;

	task_function_call(task, __perf_cgroup_move, task);
}
