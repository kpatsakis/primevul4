struct task_struct *fork_idle(int cpu)
{
	struct task_struct *task;
	task = copy_process(CLONE_VM, 0, 0, NULL, &init_struct_pid, 0, 0,
			    cpu_to_node(cpu));
	if (!IS_ERR(task)) {
		init_idle_pids(task->pids);
		init_idle(task, cpu);
	}

	return task;
}
