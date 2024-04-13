static int perf_ftrace_function_unregister(struct perf_event *event)
{
	struct ftrace_ops *ops = &event->ftrace_ops;
	int ret = unregister_ftrace_function(ops);
	ftrace_free_filter(ops);
	return ret;
}
