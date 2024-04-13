static bool is_orphaned_event(struct perf_event *event)
{
	return event->state == PERF_EVENT_STATE_DEAD;
}
