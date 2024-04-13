void sigqueue_free(struct sigqueue *q)
{
	unsigned long flags;
	spinlock_t *lock = &current->sighand->siglock;

	BUG_ON(!(q->flags & SIGQUEUE_PREALLOC));
	/*
	 * We must hold ->siglock while testing q->list
	 * to serialize with collect_signal() or with
	 * __exit_signal()->flush_sigqueue().
	 */
	spin_lock_irqsave(lock, flags);
	q->flags &= ~SIGQUEUE_PREALLOC;
	/*
	 * If it is queued it will be freed when dequeued,
	 * like the "regular" sigqueue.
	 */
	if (!list_empty(&q->list))
		q = NULL;
	spin_unlock_irqrestore(lock, flags);

	if (q)
		__sigqueue_free(q);
}
