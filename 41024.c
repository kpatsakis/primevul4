static void sync_child_event(struct perf_event *child_event,
			       struct task_struct *child)
{
	struct perf_event *parent_event = child_event->parent;
	u64 child_val;

	if (child_event->attr.inherit_stat)
		perf_event_read_event(child_event, child);

	child_val = perf_event_count(child_event);

	/*
	 * Add back the child's count to the parent's count:
	 */
	atomic64_add(child_val, &parent_event->child_count);
	atomic64_add(child_event->total_time_enabled,
		     &parent_event->child_total_time_enabled);
	atomic64_add(child_event->total_time_running,
		     &parent_event->child_total_time_running);

	/*
	 * Remove this event from the parent's list
	 */
	WARN_ON_ONCE(parent_event->ctx->parent_ctx);
	mutex_lock(&parent_event->child_mutex);
	list_del_init(&child_event->child_list);
	mutex_unlock(&parent_event->child_mutex);

	/*
	 * Release the parent event, if this was the last
	 * reference to it.
	 */
	put_event(parent_event);
}
