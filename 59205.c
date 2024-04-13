static bool is_event_hup(struct perf_event *event)
{
	bool no_children;

	if (event->state > PERF_EVENT_STATE_EXIT)
		return false;

	mutex_lock(&event->child_mutex);
	no_children = list_empty(&event->child_list);
	mutex_unlock(&event->child_mutex);
	return no_children;
}
