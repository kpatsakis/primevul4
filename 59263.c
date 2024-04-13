perf_lock_task_context(struct task_struct *task, int ctxn, unsigned long *flags)
{
	struct perf_event_context *ctx;

retry:
	/*
	 * One of the few rules of preemptible RCU is that one cannot do
	 * rcu_read_unlock() while holding a scheduler (or nested) lock when
	 * part of the read side critical section was irqs-enabled -- see
	 * rcu_read_unlock_special().
	 *
	 * Since ctx->lock nests under rq->lock we must ensure the entire read
	 * side critical section has interrupts disabled.
	 */
	local_irq_save(*flags);
	rcu_read_lock();
	ctx = rcu_dereference(task->perf_event_ctxp[ctxn]);
	if (ctx) {
		/*
		 * If this context is a clone of another, it might
		 * get swapped for another underneath us by
		 * perf_event_task_sched_out, though the
		 * rcu_read_lock() protects us from any context
		 * getting freed.  Lock the context and check if it
		 * got swapped before we could get the lock, and retry
		 * if so.  If we locked the right context, then it
		 * can't get swapped on us any more.
		 */
		raw_spin_lock(&ctx->lock);
		if (ctx != rcu_dereference(task->perf_event_ctxp[ctxn])) {
			raw_spin_unlock(&ctx->lock);
			rcu_read_unlock();
			local_irq_restore(*flags);
			goto retry;
		}

		if (ctx->task == TASK_TOMBSTONE ||
		    !atomic_inc_not_zero(&ctx->refcount)) {
			raw_spin_unlock(&ctx->lock);
			ctx = NULL;
		} else {
			WARN_ON_ONCE(ctx->task != task);
		}
	}
	rcu_read_unlock();
	if (!ctx)
		local_irq_restore(*flags);
	return ctx;
}
