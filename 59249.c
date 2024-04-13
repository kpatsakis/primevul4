int perf_event_release_kernel(struct perf_event *event)
{
	struct perf_event_context *ctx = event->ctx;
	struct perf_event *child, *tmp;

	/*
	 * If we got here through err_file: fput(event_file); we will not have
	 * attached to a context yet.
	 */
	if (!ctx) {
		WARN_ON_ONCE(event->attach_state &
				(PERF_ATTACH_CONTEXT|PERF_ATTACH_GROUP));
		goto no_ctx;
	}

	if (!is_kernel_event(event))
		perf_remove_from_owner(event);

	ctx = perf_event_ctx_lock(event);
	WARN_ON_ONCE(ctx->parent_ctx);
	perf_remove_from_context(event, DETACH_GROUP);

	raw_spin_lock_irq(&ctx->lock);
	/*
	 * Mark this even as STATE_DEAD, there is no external reference to it
	 * anymore.
	 *
	 * Anybody acquiring event->child_mutex after the below loop _must_
	 * also see this, most importantly inherit_event() which will avoid
	 * placing more children on the list.
	 *
	 * Thus this guarantees that we will in fact observe and kill _ALL_
	 * child events.
	 */
	event->state = PERF_EVENT_STATE_DEAD;
	raw_spin_unlock_irq(&ctx->lock);

	perf_event_ctx_unlock(event, ctx);

again:
	mutex_lock(&event->child_mutex);
	list_for_each_entry(child, &event->child_list, child_list) {

		/*
		 * Cannot change, child events are not migrated, see the
		 * comment with perf_event_ctx_lock_nested().
		 */
		ctx = lockless_dereference(child->ctx);
		/*
		 * Since child_mutex nests inside ctx::mutex, we must jump
		 * through hoops. We start by grabbing a reference on the ctx.
		 *
		 * Since the event cannot get freed while we hold the
		 * child_mutex, the context must also exist and have a !0
		 * reference count.
		 */
		get_ctx(ctx);

		/*
		 * Now that we have a ctx ref, we can drop child_mutex, and
		 * acquire ctx::mutex without fear of it going away. Then we
		 * can re-acquire child_mutex.
		 */
		mutex_unlock(&event->child_mutex);
		mutex_lock(&ctx->mutex);
		mutex_lock(&event->child_mutex);

		/*
		 * Now that we hold ctx::mutex and child_mutex, revalidate our
		 * state, if child is still the first entry, it didn't get freed
		 * and we can continue doing so.
		 */
		tmp = list_first_entry_or_null(&event->child_list,
					       struct perf_event, child_list);
		if (tmp == child) {
			perf_remove_from_context(child, DETACH_GROUP);
			list_del(&child->child_list);
			free_event(child);
			/*
			 * This matches the refcount bump in inherit_event();
			 * this can't be the last reference.
			 */
			put_event(event);
		}

		mutex_unlock(&event->child_mutex);
		mutex_unlock(&ctx->mutex);
		put_ctx(ctx);
		goto again;
	}
	mutex_unlock(&event->child_mutex);

no_ctx:
	put_event(event); /* Must be the 'last' reference */
	return 0;
}
