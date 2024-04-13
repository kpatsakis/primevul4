static void perf_event_context_sched_out(struct task_struct *task, int ctxn,
					 struct task_struct *next)
{
	struct perf_event_context *ctx = task->perf_event_ctxp[ctxn];
	struct perf_event_context *next_ctx;
	struct perf_event_context *parent, *next_parent;
	struct perf_cpu_context *cpuctx;
	int do_switch = 1;

	if (likely(!ctx))
		return;

	cpuctx = __get_cpu_context(ctx);
	if (!cpuctx->task_ctx)
		return;

	rcu_read_lock();
	next_ctx = next->perf_event_ctxp[ctxn];
	if (!next_ctx)
		goto unlock;

	parent = rcu_dereference(ctx->parent_ctx);
	next_parent = rcu_dereference(next_ctx->parent_ctx);

	/* If neither context have a parent context; they cannot be clones. */
	if (!parent && !next_parent)
		goto unlock;

	if (next_parent == ctx || next_ctx == parent || next_parent == parent) {
		/*
		 * Looks like the two contexts are clones, so we might be
		 * able to optimize the context switch.  We lock both
		 * contexts and check that they are clones under the
		 * lock (including re-checking that neither has been
		 * uncloned in the meantime).  It doesn't matter which
		 * order we take the locks because no other cpu could
		 * be trying to lock both of these tasks.
		 */
		raw_spin_lock(&ctx->lock);
		raw_spin_lock_nested(&next_ctx->lock, SINGLE_DEPTH_NESTING);
		if (context_equiv(ctx, next_ctx)) {
			WRITE_ONCE(ctx->task, next);
			WRITE_ONCE(next_ctx->task, task);

			swap(ctx->task_ctx_data, next_ctx->task_ctx_data);

			/*
			 * RCU_INIT_POINTER here is safe because we've not
			 * modified the ctx and the above modification of
			 * ctx->task and ctx->task_ctx_data are immaterial
			 * since those values are always verified under
			 * ctx->lock which we're now holding.
			 */
			RCU_INIT_POINTER(task->perf_event_ctxp[ctxn], next_ctx);
			RCU_INIT_POINTER(next->perf_event_ctxp[ctxn], ctx);

			do_switch = 0;

			perf_event_sync_stat(ctx, next_ctx);
		}
		raw_spin_unlock(&next_ctx->lock);
		raw_spin_unlock(&ctx->lock);
	}
unlock:
	rcu_read_unlock();

	if (do_switch) {
		raw_spin_lock(&ctx->lock);
		task_ctx_sched_out(cpuctx, ctx);
		raw_spin_unlock(&ctx->lock);
	}
}
