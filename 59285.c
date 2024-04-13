static inline int pmu_filter_match(struct perf_event *event)
{
	struct perf_event *child;

	if (!__pmu_filter_match(event))
		return 0;

	list_for_each_entry(child, &event->sibling_list, group_entry) {
		if (!__pmu_filter_match(child))
			return 0;
	}

	return 1;
}
