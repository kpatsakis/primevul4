static inline int wants_signal(int sig, struct task_struct *p)
{
	if (sigismember(&p->blocked, sig))
		return 0;
	if (p->flags & PF_EXITING)
		return 0;
	if (sig == SIGKILL)
		return 1;
	if (task_is_stopped_or_traced(p))
		return 0;
	return task_curr(p) || !signal_pending(p);
}
