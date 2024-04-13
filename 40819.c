static int do_sigpending(void *set, unsigned long sigsetsize)
{
	if (sigsetsize > sizeof(sigset_t))
		return -EINVAL;

	spin_lock_irq(&current->sighand->siglock);
	sigorsets(set, &current->pending.signal,
		  &current->signal->shared_pending.signal);
	spin_unlock_irq(&current->sighand->siglock);

	/* Outside the lock because only this thread touches it.  */
	sigandsets(set, &current->blocked, set);
	return 0;
}
