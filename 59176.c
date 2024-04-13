static void __perf_event_disable(struct perf_event *event,
				 struct perf_cpu_context *cpuctx,
				 struct perf_event_context *ctx,
				 void *info)
{
	if (event->state < PERF_EVENT_STATE_INACTIVE)
		return;

	update_context_time(ctx);
	update_cgrp_time_from_event(event);
	update_group_times(event);
	if (event == event->group_leader)
		group_sched_out(event, cpuctx, ctx);
	else
		event_sched_out(event, cpuctx, ctx);
	event->state = PERF_EVENT_STATE_OFF;
}
