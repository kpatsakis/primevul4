int send_sigqueue(struct sigqueue *q, struct task_struct *t, int group)
{
	int sig = q->info.si_signo;
	struct sigpending *pending;
	unsigned long flags;
	int ret, result;

	BUG_ON(!(q->flags & SIGQUEUE_PREALLOC));

	ret = -1;
	if (!likely(lock_task_sighand(t, &flags)))
		goto ret;

	ret = 1; /* the signal is ignored */
	result = TRACE_SIGNAL_IGNORED;
	if (!prepare_signal(sig, t, false))
		goto out;

	ret = 0;
	if (unlikely(!list_empty(&q->list))) {
		/*
		 * If an SI_TIMER entry is already queue just increment
		 * the overrun count.
		 */
		BUG_ON(q->info.si_code != SI_TIMER);
		q->info.si_overrun++;
		result = TRACE_SIGNAL_ALREADY_PENDING;
		goto out;
	}
	q->info.si_overrun = 0;

	signalfd_notify(t, sig);
	pending = group ? &t->signal->shared_pending : &t->pending;
	list_add_tail(&q->list, &pending->list);
	sigaddset(&pending->signal, sig);
	complete_signal(sig, t, group);
	result = TRACE_SIGNAL_DELIVERED;
out:
	trace_signal_generate(sig, &q->info, t, group, result);
	unlock_task_sighand(t, &flags);
ret:
	return ret;
}
