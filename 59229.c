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
				 overflow_handler, context, -1);
	if (IS_ERR(event)) {
		err = PTR_ERR(event);
		goto err;
	}

	/* Mark owner so we could distinguish it from user events. */
	event->owner = TASK_TOMBSTONE;

	ctx = find_get_context(event->pmu, task, event);
	if (IS_ERR(ctx)) {
		err = PTR_ERR(ctx);
		goto err_free;
	}

	WARN_ON_ONCE(ctx->parent_ctx);
	mutex_lock(&ctx->mutex);
	if (ctx->task == TASK_TOMBSTONE) {
		err = -ESRCH;
		goto err_unlock;
	}

	if (!exclusive_event_installable(event, ctx)) {
		err = -EBUSY;
		goto err_unlock;
	}

	perf_install_in_context(ctx, event, cpu);
	perf_unpin_context(ctx);
	mutex_unlock(&ctx->mutex);

	return event;

err_unlock:
	mutex_unlock(&ctx->mutex);
	perf_unpin_context(ctx);
	put_ctx(ctx);
err_free:
	free_event(event);
err:
	return ERR_PTR(err);
}
