static void perf_event_enable_on_exec(int ctxn)
{
	struct perf_event_context *ctx, *clone_ctx = NULL;
	struct perf_cpu_context *cpuctx;
	struct perf_event *event;
	unsigned long flags;
	int enabled = 0;

	local_irq_save(flags);
	ctx = current->perf_event_ctxp[ctxn];
	if (!ctx || !ctx->nr_events)
		goto out;

	cpuctx = __get_cpu_context(ctx);
	perf_ctx_lock(cpuctx, ctx);
	ctx_sched_out(ctx, cpuctx, EVENT_TIME);
	list_for_each_entry(event, &ctx->event_list, event_entry)
		enabled |= event_enable_on_exec(event, ctx);

	/*
	 * Unclone and reschedule this context if we enabled any event.
	 */
	if (enabled) {
		clone_ctx = unclone_ctx(ctx);
		ctx_resched(cpuctx, ctx);
	}
	perf_ctx_unlock(cpuctx, ctx);

out:
	local_irq_restore(flags);

	if (clone_ctx)
		put_ctx(clone_ctx);
}
