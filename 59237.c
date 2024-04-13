static void perf_event_exit_task_context(struct task_struct *child, int ctxn)
{
	struct perf_event_context *child_ctx, *clone_ctx = NULL;
	struct perf_event *child_event, *next;

	WARN_ON_ONCE(child != current);

	child_ctx = perf_pin_task_context(child, ctxn);
	if (!child_ctx)
		return;

	/*
	 * In order to reduce the amount of tricky in ctx tear-down, we hold
	 * ctx::mutex over the entire thing. This serializes against almost
	 * everything that wants to access the ctx.
	 *
	 * The exception is sys_perf_event_open() /
	 * perf_event_create_kernel_count() which does find_get_context()
	 * without ctx::mutex (it cannot because of the move_group double mutex
	 * lock thing). See the comments in perf_install_in_context().
	 */
	mutex_lock(&child_ctx->mutex);

	/*
	 * In a single ctx::lock section, de-schedule the events and detach the
	 * context from the task such that we cannot ever get it scheduled back
	 * in.
	 */
	raw_spin_lock_irq(&child_ctx->lock);
	task_ctx_sched_out(__get_cpu_context(child_ctx), child_ctx);

	/*
	 * Now that the context is inactive, destroy the task <-> ctx relation
	 * and mark the context dead.
	 */
	RCU_INIT_POINTER(child->perf_event_ctxp[ctxn], NULL);
	put_ctx(child_ctx); /* cannot be last */
	WRITE_ONCE(child_ctx->task, TASK_TOMBSTONE);
	put_task_struct(current); /* cannot be last */

	clone_ctx = unclone_ctx(child_ctx);
	raw_spin_unlock_irq(&child_ctx->lock);

	if (clone_ctx)
		put_ctx(clone_ctx);

	/*
	 * Report the task dead after unscheduling the events so that we
	 * won't get any samples after PERF_RECORD_EXIT. We can however still
	 * get a few PERF_RECORD_READ events.
	 */
	perf_event_task(child, child_ctx, 0);

	list_for_each_entry_safe(child_event, next, &child_ctx->event_list, event_entry)
		perf_event_exit_event(child_event, child_ctx, child);

	mutex_unlock(&child_ctx->mutex);

	put_ctx(child_ctx);
}
