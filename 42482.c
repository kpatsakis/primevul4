void set_task_comm(struct task_struct *tsk, char *buf)
{
	task_lock(tsk);

	trace_task_rename(tsk, buf);

	/*
	 * Threads may access current->comm without holding
	 * the task lock, so write the string carefully.
	 * Readers without a lock may see incomplete new
	 * names but are safe from non-terminating string reads.
	 */
	memset(tsk->comm, 0, TASK_COMM_LEN);
	wmb();
	strlcpy(tsk->comm, buf, sizeof(tsk->comm));
	task_unlock(tsk);
	perf_event_comm(tsk);
}
