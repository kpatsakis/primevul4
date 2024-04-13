void set_task_comm(struct task_struct *tsk, char *buf)
{
	task_lock(tsk);
	trace_task_rename(tsk, buf);
	strlcpy(tsk->comm, buf, sizeof(tsk->comm));
	task_unlock(tsk);
	perf_event_comm(tsk);
}
