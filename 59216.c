perf_addr_filter_new(struct perf_event *event, struct list_head *filters)
{
	int node = cpu_to_node(event->cpu == -1 ? 0 : event->cpu);
	struct perf_addr_filter *filter;

	filter = kzalloc_node(sizeof(*filter), GFP_KERNEL, node);
	if (!filter)
		return NULL;

	INIT_LIST_HEAD(&filter->entry);
	list_add_tail(&filter->entry, filters);

	return filter;
}
