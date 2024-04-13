static int sigkill_pending(struct task_struct *tsk)
{
	return	sigismember(&tsk->pending.signal, SIGKILL) ||
		sigismember(&tsk->signal->shared_pending.signal, SIGKILL);
}
