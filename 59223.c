static void perf_event_addr_filters_exec(struct perf_event *event, void *data)
{
	struct perf_addr_filters_head *ifh = perf_event_addr_filters(event);
	struct perf_addr_filter *filter;
	unsigned int restart = 0, count = 0;
	unsigned long flags;

	if (!has_addr_filter(event))
		return;

	raw_spin_lock_irqsave(&ifh->lock, flags);
	list_for_each_entry(filter, &ifh->list, entry) {
		if (filter->inode) {
			event->addr_filters_offs[count] = 0;
			restart++;
		}

		count++;
	}

	if (restart)
		event->addr_filters_gen++;
	raw_spin_unlock_irqrestore(&ifh->lock, flags);

	if (restart)
		perf_event_stop(event, 1);
}
