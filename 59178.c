static void __perf_event_exit_context(void *__info)
{
	struct perf_event_context *ctx = __info;
	struct perf_cpu_context *cpuctx = __get_cpu_context(ctx);
	struct perf_event *event;

	raw_spin_lock(&ctx->lock);
	list_for_each_entry(event, &ctx->event_list, event_entry)
		__perf_remove_from_context(event, cpuctx, ctx, (void *)DETACH_GROUP);
	raw_spin_unlock(&ctx->lock);
}
