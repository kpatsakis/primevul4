static void put_ctx(struct perf_event_context *ctx)
{
	if (atomic_dec_and_test(&ctx->refcount)) {
		if (ctx->parent_ctx)
			put_ctx(ctx->parent_ctx);
		if (ctx->task && ctx->task != TASK_TOMBSTONE)
			put_task_struct(ctx->task);
		call_rcu(&ctx->rcu_head, free_ctx);
	}
}
