static void event_function_call(struct perf_event *event, event_f func, void *data)
{
	struct perf_event_context *ctx = event->ctx;
	struct task_struct *task = READ_ONCE(ctx->task); /* verified in event_function */
	struct event_function_struct efs = {
		.event = event,
		.func = func,
		.data = data,
	};

	if (!event->parent) {
		/*
		 * If this is a !child event, we must hold ctx::mutex to
		 * stabilize the the event->ctx relation. See
		 * perf_event_ctx_lock().
		 */
		lockdep_assert_held(&ctx->mutex);
	}

	if (!task) {
		cpu_function_call(event->cpu, event_function, &efs);
		return;
	}

	if (task == TASK_TOMBSTONE)
		return;

again:
	if (!task_function_call(task, event_function, &efs))
		return;

	raw_spin_lock_irq(&ctx->lock);
	/*
	 * Reload the task pointer, it might have been changed by
	 * a concurrent perf_event_context_sched_out().
	 */
	task = ctx->task;
	if (task == TASK_TOMBSTONE) {
		raw_spin_unlock_irq(&ctx->lock);
		return;
	}
	if (ctx->is_active) {
		raw_spin_unlock_irq(&ctx->lock);
		goto again;
	}
	func(event, NULL, ctx, data);
	raw_spin_unlock_irq(&ctx->lock);
}
