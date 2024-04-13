static int task_clock_event_init(struct perf_event *event)
{
	if (event->attr.type != PERF_TYPE_SOFTWARE)
		return -ENOENT;

	if (event->attr.config != PERF_COUNT_SW_TASK_CLOCK)
		return -ENOENT;

	/*
	 * no branch sampling for software events
	 */
	if (has_branch_stack(event))
		return -EOPNOTSUPP;

	perf_swevent_init_hrtimer(event);

	return 0;
}
