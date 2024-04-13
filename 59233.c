void perf_event_exec(void)
{
	struct perf_event_context *ctx;
	int ctxn;

	rcu_read_lock();
	for_each_task_context_nr(ctxn) {
		ctx = current->perf_event_ctxp[ctxn];
		if (!ctx)
			continue;

		perf_event_enable_on_exec(ctxn);

		perf_iterate_ctx(ctx, perf_event_addr_filters_exec, NULL,
				   true);
	}
	rcu_read_unlock();
}
