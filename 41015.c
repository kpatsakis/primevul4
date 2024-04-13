static int perf_tp_event_init(struct perf_event *event)
{
	int err;

	if (event->attr.type != PERF_TYPE_TRACEPOINT)
		return -ENOENT;

	/*
	 * no branch sampling for tracepoint events
	 */
	if (has_branch_stack(event))
		return -EOPNOTSUPP;

	err = perf_trace_init(event);
	if (err)
		return err;

	event->destroy = tp_perf_event_destroy;

	return 0;
}
