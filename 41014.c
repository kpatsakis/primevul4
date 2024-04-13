void perf_tp_event(u64 addr, u64 count, void *record, int entry_size,
		   struct pt_regs *regs, struct hlist_head *head, int rctx,
		   struct task_struct *task)
{
	struct perf_sample_data data;
	struct perf_event *event;

	struct perf_raw_record raw = {
		.size = entry_size,
		.data = record,
	};

	perf_sample_data_init(&data, addr, 0);
	data.raw = &raw;

	hlist_for_each_entry_rcu(event, head, hlist_entry) {
		if (perf_tp_event_match(event, &data, regs))
			perf_swevent_event(event, count, &data, regs);
	}

	/*
	 * If we got specified a target task, also iterate its context and
	 * deliver this event there too.
	 */
	if (task && task != current) {
		struct perf_event_context *ctx;
		struct trace_entry *entry = record;

		rcu_read_lock();
		ctx = rcu_dereference(task->perf_event_ctxp[perf_sw_context]);
		if (!ctx)
			goto unlock;

		list_for_each_entry_rcu(event, &ctx->event_list, event_entry) {
			if (event->attr.type != PERF_TYPE_TRACEPOINT)
				continue;
			if (event->attr.config != entry->type)
				continue;
			if (perf_tp_event_match(event, &data, regs))
				perf_swevent_event(event, count, &data, regs);
		}
unlock:
		rcu_read_unlock();
	}

	perf_swevent_put_recursion_context(rctx);
}
