list_del_event(struct perf_event *event, struct perf_event_context *ctx)
{
	struct perf_cpu_context *cpuctx;
	/*
	 * We can have double detach due to exit/hot-unplug + close.
	 */
	if (!(event->attach_state & PERF_ATTACH_CONTEXT))
		return;

	event->attach_state &= ~PERF_ATTACH_CONTEXT;

	if (is_cgroup_event(event)) {
		ctx->nr_cgroups--;
		cpuctx = __get_cpu_context(ctx);
		/*
		 * if there are no more cgroup events
		 * then cler cgrp to avoid stale pointer
		 * in update_cgrp_time_from_cpuctx()
		 */
		if (!ctx->nr_cgroups)
			cpuctx->cgrp = NULL;
	}

	if (has_branch_stack(event))
		ctx->nr_branch_stack--;

	ctx->nr_events--;
	if (event->attr.inherit_stat)
		ctx->nr_stat--;

	list_del_rcu(&event->event_entry);

	if (event->group_leader == event)
		list_del_init(&event->group_entry);

	update_group_times(event);

	/*
	 * If event was in error state, then keep it
	 * that way, otherwise bogus counts will be
	 * returned on read(). The only way to get out
	 * of error state is by explicit re-enabling
	 * of the event
	 */
	if (event->state > PERF_EVENT_STATE_OFF)
		event->state = PERF_EVENT_STATE_OFF;
}
