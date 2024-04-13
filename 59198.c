static void event_function_local(struct perf_event *event, event_f func, void *data)
{
	struct perf_event_context *ctx = event->ctx;
	struct perf_cpu_context *cpuctx = __get_cpu_context(ctx);
	struct task_struct *task = READ_ONCE(ctx->task);
	struct perf_event_context *task_ctx = NULL;

	WARN_ON_ONCE(!irqs_disabled());

	if (task) {
		if (task == TASK_TOMBSTONE)
			return;

		task_ctx = ctx;
	}

	perf_ctx_lock(cpuctx, task_ctx);

	task = ctx->task;
	if (task == TASK_TOMBSTONE)
		goto unlock;

	if (task) {
		/*
		 * We must be either inactive or active and the right task,
		 * otherwise we're screwed, since we cannot IPI to somewhere
		 * else.
		 */
		if (ctx->is_active) {
			if (WARN_ON_ONCE(task != current))
				goto unlock;

			if (WARN_ON_ONCE(cpuctx->task_ctx != ctx))
				goto unlock;
		}
	} else {
		WARN_ON_ONCE(&cpuctx->ctx != ctx);
	}

	func(event, cpuctx, ctx, data);
unlock:
	perf_ctx_unlock(cpuctx, task_ctx);
}
