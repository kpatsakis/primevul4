ctx_sched_in(struct perf_event_context *ctx,
	     struct perf_cpu_context *cpuctx,
	     enum event_type_t event_type,
	     struct task_struct *task)
{
	int is_active = ctx->is_active;
	u64 now;

	lockdep_assert_held(&ctx->lock);

	if (likely(!ctx->nr_events))
		return;

	ctx->is_active |= (event_type | EVENT_TIME);
	if (ctx->task) {
		if (!is_active)
			cpuctx->task_ctx = ctx;
		else
			WARN_ON_ONCE(cpuctx->task_ctx != ctx);
	}

	is_active ^= ctx->is_active; /* changed bits */

	if (is_active & EVENT_TIME) {
		/* start ctx time */
		now = perf_clock();
		ctx->timestamp = now;
		perf_cgroup_set_timestamp(task, ctx);
	}

	/*
	 * First go through the list and put on any pinned groups
	 * in order to give them the best chance of going on.
	 */
	if (is_active & EVENT_PINNED)
		ctx_pinned_sched_in(ctx, cpuctx);

	/* Then walk through the lower prio flexible groups */
	if (is_active & EVENT_FLEXIBLE)
		ctx_flexible_sched_in(ctx, cpuctx);
}
