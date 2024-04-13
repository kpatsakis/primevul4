perf_iterate_sb(perf_iterate_f output, void *data,
	       struct perf_event_context *task_ctx)
{
	struct perf_event_context *ctx;
	int ctxn;

	rcu_read_lock();
	preempt_disable();

	/*
	 * If we have task_ctx != NULL we only notify the task context itself.
	 * The task_ctx is set only for EXIT events before releasing task
	 * context.
	 */
	if (task_ctx) {
		perf_iterate_ctx(task_ctx, output, data, false);
		goto done;
	}

	perf_iterate_sb_cpu(output, data);

	for_each_task_context_nr(ctxn) {
		ctx = rcu_dereference(current->perf_event_ctxp[ctxn]);
		if (ctx)
			perf_iterate_ctx(ctx, output, data, false);
	}
done:
	preempt_enable();
	rcu_read_unlock();
}
