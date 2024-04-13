list_update_cgroup_event(struct perf_event *event,
			 struct perf_event_context *ctx, bool add)
{
	struct perf_cpu_context *cpuctx;

	if (!is_cgroup_event(event))
		return;

	if (add && ctx->nr_cgroups++)
		return;
	else if (!add && --ctx->nr_cgroups)
		return;
	/*
	 * Because cgroup events are always per-cpu events,
	 * this will always be called from the right CPU.
	 */
	cpuctx = __get_cpu_context(ctx);

	/*
	 * cpuctx->cgrp is NULL until a cgroup event is sched in or
	 * ctx->nr_cgroup == 0 .
	 */
	if (add && perf_cgroup_from_task(current, ctx) == event->cgrp)
		cpuctx->cgrp = event->cgrp;
	else if (!add)
		cpuctx->cgrp = NULL;
}
