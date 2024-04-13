static void perf_event_addr_filters_apply(struct perf_event *event)
{
	struct perf_addr_filters_head *ifh = perf_event_addr_filters(event);
	struct task_struct *task = READ_ONCE(event->ctx->task);
	struct perf_addr_filter *filter;
	struct mm_struct *mm = NULL;
	unsigned int count = 0;
	unsigned long flags;

	/*
	 * We may observe TASK_TOMBSTONE, which means that the event tear-down
	 * will stop on the parent's child_mutex that our caller is also holding
	 */
	if (task == TASK_TOMBSTONE)
		return;

	mm = get_task_mm(event->ctx->task);
	if (!mm)
		goto restart;

	down_read(&mm->mmap_sem);

	raw_spin_lock_irqsave(&ifh->lock, flags);
	list_for_each_entry(filter, &ifh->list, entry) {
		event->addr_filters_offs[count] = 0;

		/*
		 * Adjust base offset if the filter is associated to a binary
		 * that needs to be mapped:
		 */
		if (filter->inode)
			event->addr_filters_offs[count] =
				perf_addr_filter_apply(filter, mm);

		count++;
	}

	event->addr_filters_gen++;
	raw_spin_unlock_irqrestore(&ifh->lock, flags);

	up_read(&mm->mmap_sem);

	mmput(mm);

restart:
	perf_event_stop(event, 1);
}
