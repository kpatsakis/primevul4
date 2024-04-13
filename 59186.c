__perf_remove_from_context(struct perf_event *event,
			   struct perf_cpu_context *cpuctx,
			   struct perf_event_context *ctx,
			   void *info)
{
	unsigned long flags = (unsigned long)info;

	event_sched_out(event, cpuctx, ctx);
	if (flags & DETACH_GROUP)
		perf_group_detach(event);
	list_del_event(event, ctx);

	if (!ctx->nr_events && ctx->is_active) {
		ctx->is_active = 0;
		if (ctx->task) {
			WARN_ON_ONCE(cpuctx->task_ctx != ctx);
			cpuctx->task_ctx = NULL;
		}
	}
}
