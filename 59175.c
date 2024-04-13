static void __perf_addr_filters_adjust(struct perf_event *event, void *data)
{
	struct perf_addr_filters_head *ifh = perf_event_addr_filters(event);
	struct vm_area_struct *vma = data;
	unsigned long off = vma->vm_pgoff << PAGE_SHIFT, flags;
	struct file *file = vma->vm_file;
	struct perf_addr_filter *filter;
	unsigned int restart = 0, count = 0;

	if (!has_addr_filter(event))
		return;

	if (!file)
		return;

	raw_spin_lock_irqsave(&ifh->lock, flags);
	list_for_each_entry(filter, &ifh->list, entry) {
		if (perf_addr_filter_match(filter, file, off,
					     vma->vm_end - vma->vm_start)) {
			event->addr_filters_offs[count] = vma->vm_start;
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
