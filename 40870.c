int unhandled_signal(struct task_struct *tsk, int sig)
{
	void __user *handler = tsk->sighand->action[sig-1].sa.sa_handler;
	if (is_global_init(tsk))
		return 1;
	if (handler != SIG_IGN && handler != SIG_DFL)
		return 0;
	/* if ptraced, let the tracer determine */
	return !tsk->ptrace;
}
