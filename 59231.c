void perf_event_disable_local(struct perf_event *event)
{
	event_function_local(event, __perf_event_disable, NULL);
}
