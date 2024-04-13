void perf_pmu_migrate_context(struct pmu *pmu, int src_cpu, int dst_cpu)
{
	struct perf_event_context *src_ctx;
	struct perf_event_context *dst_ctx;
	struct perf_event *event, *tmp;
	LIST_HEAD(events);

	src_ctx = &per_cpu_ptr(pmu->pmu_cpu_context, src_cpu)->ctx;
	dst_ctx = &per_cpu_ptr(pmu->pmu_cpu_context, dst_cpu)->ctx;

	/*
	 * See perf_event_ctx_lock() for comments on the details
	 * of swizzling perf_event::ctx.
	 */
	mutex_lock_double(&src_ctx->mutex, &dst_ctx->mutex);
	list_for_each_entry_safe(event, tmp, &src_ctx->event_list,
				 event_entry) {
		perf_remove_from_context(event, 0);
		unaccount_event_cpu(event, src_cpu);
		put_ctx(src_ctx);
		list_add(&event->migrate_entry, &events);
	}

	/*
	 * Wait for the events to quiesce before re-instating them.
	 */
	synchronize_rcu();

	/*
	 * Re-instate events in 2 passes.
	 *
	 * Skip over group leaders and only install siblings on this first
	 * pass, siblings will not get enabled without a leader, however a
	 * leader will enable its siblings, even if those are still on the old
	 * context.
	 */
	list_for_each_entry_safe(event, tmp, &events, migrate_entry) {
		if (event->group_leader == event)
			continue;

		list_del(&event->migrate_entry);
		if (event->state >= PERF_EVENT_STATE_OFF)
			event->state = PERF_EVENT_STATE_INACTIVE;
		account_event_cpu(event, dst_cpu);
		perf_install_in_context(dst_ctx, event, dst_cpu);
		get_ctx(dst_ctx);
	}

	/*
	 * Once all the siblings are setup properly, install the group leaders
	 * to make it go.
	 */
	list_for_each_entry_safe(event, tmp, &events, migrate_entry) {
		list_del(&event->migrate_entry);
		if (event->state >= PERF_EVENT_STATE_OFF)
			event->state = PERF_EVENT_STATE_INACTIVE;
		account_event_cpu(event, dst_cpu);
		perf_install_in_context(dst_ctx, event, dst_cpu);
		get_ctx(dst_ctx);
	}
	mutex_unlock(&dst_ctx->mutex);
	mutex_unlock(&src_ctx->mutex);
}
