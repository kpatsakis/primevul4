perf_event_set_addr_filter(struct perf_event *event, char *filter_str)
{
	LIST_HEAD(filters);
	int ret;

	/*
	 * Since this is called in perf_ioctl() path, we're already holding
	 * ctx::mutex.
	 */
	lockdep_assert_held(&event->ctx->mutex);

	if (WARN_ON_ONCE(event->parent))
		return -EINVAL;

	/*
	 * For now, we only support filtering in per-task events; doing so
	 * for CPU-wide events requires additional context switching trickery,
	 * since same object code will be mapped at different virtual
	 * addresses in different processes.
	 */
	if (!event->ctx->task)
		return -EOPNOTSUPP;

	ret = perf_event_parse_addr_filter(event, filter_str, &filters);
	if (ret)
		return ret;

	ret = event->pmu->addr_filters_validate(&filters);
	if (ret) {
		free_filters_list(&filters);
		return ret;
	}

	/* remove existing filters, if any */
	perf_addr_filters_splice(event, &filters);

	/* install new filters */
	perf_event_for_each_child(event, perf_event_addr_filters_apply);

	return ret;
}
