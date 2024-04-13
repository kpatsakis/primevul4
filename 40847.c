static void retarget_shared_pending(struct task_struct *tsk, sigset_t *which)
{
	sigset_t retarget;
	struct task_struct *t;

	sigandsets(&retarget, &tsk->signal->shared_pending.signal, which);
	if (sigisemptyset(&retarget))
		return;

	t = tsk;
	while_each_thread(tsk, t) {
		if (t->flags & PF_EXITING)
			continue;

		if (!has_pending_signals(&retarget, &t->blocked))
			continue;
		/* Remove the signals this thread can handle. */
		sigandsets(&retarget, &retarget, &t->blocked);

		if (!signal_pending(t))
			signal_wake_up(t, 0);

		if (sigisemptyset(&retarget))
			break;
	}
}
