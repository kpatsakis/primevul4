list_add_event(struct perf_event *event, struct perf_event_context *ctx)
{

	lockdep_assert_held(&ctx->lock);

	WARN_ON_ONCE(event->attach_state & PERF_ATTACH_CONTEXT);
	event->attach_state |= PERF_ATTACH_CONTEXT;

	/*
	 * If we're a stand alone event or group leader, we go to the context
	 * list, group events are kept attached to the group so that
	 * perf_group_detach can, at all times, locate all siblings.
	 */
	if (event->group_leader == event) {
		struct list_head *list;

		event->group_caps = event->event_caps;

		list = ctx_group_list(event, ctx);
		list_add_tail(&event->group_entry, list);
	}

	list_update_cgroup_event(event, ctx, true);

	list_add_rcu(&event->event_entry, &ctx->event_list);
	ctx->nr_events++;
	if (event->attr.inherit_stat)
		ctx->nr_stat++;

	ctx->generation++;
}
