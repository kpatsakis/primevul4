static int perf_event_set_filter(struct perf_event *event, void __user *arg)
{
	char *filter_str;
	int ret = -EINVAL;

	if ((event->attr.type != PERF_TYPE_TRACEPOINT ||
	    !IS_ENABLED(CONFIG_EVENT_TRACING)) &&
	    !has_addr_filter(event))
		return -EINVAL;

	filter_str = strndup_user(arg, PAGE_SIZE);
	if (IS_ERR(filter_str))
		return PTR_ERR(filter_str);

	if (IS_ENABLED(CONFIG_EVENT_TRACING) &&
	    event->attr.type == PERF_TYPE_TRACEPOINT)
		ret = ftrace_profile_set_filter(event, event->attr.config,
						filter_str);
	else if (has_addr_filter(event))
		ret = perf_event_set_addr_filter(event, filter_str);

	kfree(filter_str);
	return ret;
}
