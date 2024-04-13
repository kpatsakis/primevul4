int wake_up_process(struct task_struct *p)
{
	WARN_ON(task_is_stopped_or_traced(p));
	return try_to_wake_up(p, TASK_NORMAL, 0);
}
