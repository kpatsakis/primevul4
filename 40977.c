perf_event_create_kernel_counter(struct perf_event_attr *attr, int cpu,
				 struct task_struct *task,
				 perf_overflow_handler_t overflow_handler,
				 void *context)
{
	struct perf_event_context *ctx;
	struct perf_event *event;
	int err;

	/*
	 * Get the target context (task or percpu):
	 */

	event = perf_event_alloc(attr, cpu, task, NULL, NULL,
				 overflow_handler, context);
	if (IS_ERR(event)) {
		err = PTR_ERR(event);
		goto err;
	}

	ctx = find_get_context(event->pmu, task, cpu);
	if (IS_ERR(ctx)) {
		err = PTR_ERR(ctx);
		goto err_free;
	}

	WARN_ON_ONCE(ctx->parent_ctx);
	mutex_lock(&ctx->mutex);
	perf_install_in_context(ctx, event, cpu);
	++ctx->generation;
	perf_unpin_context(ctx);
	mutex_unlock(&ctx->mutex);

	return event;

err_free:
	free_event(event);
err:
	return ERR_PTR(err);
}
