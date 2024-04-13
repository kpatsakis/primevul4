static void __perf_event_mark_enabled(struct perf_event *event)
{
	struct perf_event *sub;
	u64 tstamp = perf_event_time(event);

	event->state = PERF_EVENT_STATE_INACTIVE;
	event->tstamp_enabled = tstamp - event->total_time_enabled;
	list_for_each_entry(sub, &event->sibling_list, group_entry) {
		if (sub->state >= PERF_EVENT_STATE_INACTIVE)
			sub->tstamp_enabled = tstamp - sub->total_time_enabled;
	}
}
