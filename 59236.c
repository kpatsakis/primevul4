perf_event_exit_event(struct perf_event *child_event,
		      struct perf_event_context *child_ctx,
		      struct task_struct *child)
{
	struct perf_event *parent_event = child_event->parent;

	/*
	 * Do not destroy the 'original' grouping; because of the context
	 * switch optimization the original events could've ended up in a
	 * random child task.
	 *
	 * If we were to destroy the original group, all group related
	 * operations would cease to function properly after this random
	 * child dies.
	 *
	 * Do destroy all inherited groups, we don't care about those
	 * and being thorough is better.
	 */
	raw_spin_lock_irq(&child_ctx->lock);
	WARN_ON_ONCE(child_ctx->is_active);

	if (parent_event)
		perf_group_detach(child_event);
	list_del_event(child_event, child_ctx);
	child_event->state = PERF_EVENT_STATE_EXIT; /* is_event_hup() */
	raw_spin_unlock_irq(&child_ctx->lock);

	/*
	 * Parent events are governed by their filedesc, retain them.
	 */
	if (!parent_event) {
		perf_event_wakeup(child_event);
		return;
	}
	/*
	 * Child events can be cleaned up.
	 */

	sync_child_event(child_event, child);

	/*
	 * Remove this event from the parent's list
	 */
	WARN_ON_ONCE(parent_event->ctx->parent_ctx);
	mutex_lock(&parent_event->child_mutex);
	list_del_init(&child_event->child_list);
	mutex_unlock(&parent_event->child_mutex);

	/*
	 * Kick perf_poll() for is_event_hup().
	 */
	perf_event_wakeup(parent_event);
	free_event(child_event);
	put_event(parent_event);
}
