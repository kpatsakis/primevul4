static void __perf_event_enable(struct perf_event *event,
				struct perf_cpu_context *cpuctx,
				struct perf_event_context *ctx,
				void *info)
{
	struct perf_event *leader = event->group_leader;
	struct perf_event_context *task_ctx;

	if (event->state >= PERF_EVENT_STATE_INACTIVE ||
	    event->state <= PERF_EVENT_STATE_ERROR)
		return;

	if (ctx->is_active)
		ctx_sched_out(ctx, cpuctx, EVENT_TIME);

	__perf_event_mark_enabled(event);

	if (!ctx->is_active)
		return;

	if (!event_filter_match(event)) {
		if (is_cgroup_event(event))
			perf_cgroup_defer_enabled(event);
		ctx_sched_in(ctx, cpuctx, EVENT_TIME, current);
		return;
	}

	/*
	 * If the event is in a group and isn't the group leader,
	 * then don't put it on unless the group is on.
	 */
	if (leader != event && leader->state != PERF_EVENT_STATE_ACTIVE) {
		ctx_sched_in(ctx, cpuctx, EVENT_TIME, current);
		return;
	}

	task_ctx = cpuctx->task_ctx;
	if (ctx->task)
		WARN_ON_ONCE(task_ctx != ctx);

	ctx_resched(cpuctx, task_ctx);
}
