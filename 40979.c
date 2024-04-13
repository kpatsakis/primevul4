static void perf_event_exit_task_context(struct task_struct *child, int ctxn)
{
	struct perf_event *child_event, *tmp;
	struct perf_event_context *child_ctx;
	unsigned long flags;

	if (likely(!child->perf_event_ctxp[ctxn])) {
		perf_event_task(child, NULL, 0);
		return;
	}

	local_irq_save(flags);
	/*
	 * We can't reschedule here because interrupts are disabled,
	 * and either child is current or it is a task that can't be
	 * scheduled, so we are now safe from rescheduling changing
	 * our context.
	 */
	child_ctx = rcu_dereference_raw(child->perf_event_ctxp[ctxn]);

	/*
	 * Take the context lock here so that if find_get_context is
	 * reading child->perf_event_ctxp, we wait until it has
	 * incremented the context's refcount before we do put_ctx below.
	 */
	raw_spin_lock(&child_ctx->lock);
	task_ctx_sched_out(child_ctx);
	child->perf_event_ctxp[ctxn] = NULL;
	/*
	 * If this context is a clone; unclone it so it can't get
	 * swapped to another process while we're removing all
	 * the events from it.
	 */
	unclone_ctx(child_ctx);
	update_context_time(child_ctx);
	raw_spin_unlock_irqrestore(&child_ctx->lock, flags);

	/*
	 * Report the task dead after unscheduling the events so that we
	 * won't get any samples after PERF_RECORD_EXIT. We can however still
	 * get a few PERF_RECORD_READ events.
	 */
	perf_event_task(child, child_ctx, 0);

	/*
	 * We can recurse on the same lock type through:
	 *
	 *   __perf_event_exit_task()
	 *     sync_child_event()
	 *       put_event()
	 *         mutex_lock(&ctx->mutex)
	 *
	 * But since its the parent context it won't be the same instance.
	 */
	mutex_lock(&child_ctx->mutex);

again:
	list_for_each_entry_safe(child_event, tmp, &child_ctx->pinned_groups,
				 group_entry)
		__perf_event_exit_task(child_event, child_ctx, child);

	list_for_each_entry_safe(child_event, tmp, &child_ctx->flexible_groups,
				 group_entry)
		__perf_event_exit_task(child_event, child_ctx, child);

	/*
	 * If the last event was a group event, it will have appended all
	 * its siblings to the list, but we obtained 'tmp' before that which
	 * will still point to the list head terminating the iteration.
	 */
	if (!list_empty(&child_ctx->pinned_groups) ||
	    !list_empty(&child_ctx->flexible_groups))
		goto again;

	mutex_unlock(&child_ctx->mutex);

	put_ctx(child_ctx);
}
