static void perf_event_context_sched_in(struct perf_event_context *ctx,
					struct task_struct *task)
{
	struct perf_cpu_context *cpuctx;

	cpuctx = __get_cpu_context(ctx);
	if (cpuctx->task_ctx == ctx)
		return;

	perf_ctx_lock(cpuctx, ctx);
	perf_pmu_disable(ctx->pmu);
	/*
	 * We want to keep the following priority order:
	 * cpu pinned (that don't need to move), task pinned,
	 * cpu flexible, task flexible.
	 */
	cpu_ctx_sched_out(cpuctx, EVENT_FLEXIBLE);

	if (ctx->nr_events)
		cpuctx->task_ctx = ctx;

	perf_event_sched_in(cpuctx, cpuctx->task_ctx, task);

	perf_pmu_enable(ctx->pmu);
	perf_ctx_unlock(cpuctx, ctx);

	/*
	 * Since these rotations are per-cpu, we need to ensure the
	 * cpu-context we got scheduled on is actually rotating.
	 */
	perf_pmu_rotate_start(ctx->pmu);
}
