perf_iterate_ctx(struct perf_event_context *ctx,
		   perf_iterate_f output,
		   void *data, bool all)
{
	struct perf_event *event;

	list_for_each_entry_rcu(event, &ctx->event_list, event_entry) {
		if (!all) {
			if (event->state < PERF_EVENT_STATE_INACTIVE)
				continue;
			if (!event_filter_match(event))
				continue;
		}

		output(event, data);
	}
}
