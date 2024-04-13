static int perf_event_read(struct perf_event *event, bool group)
{
	int ret = 0, cpu_to_read, local_cpu;

	/*
	 * If event is enabled and currently active on a CPU, update the
	 * value in the event structure:
	 */
	if (event->state == PERF_EVENT_STATE_ACTIVE) {
		struct perf_read_data data = {
			.event = event,
			.group = group,
			.ret = 0,
		};

		local_cpu = get_cpu();
		cpu_to_read = find_cpu_to_read(event, local_cpu);
		put_cpu();

		/*
		 * Purposely ignore the smp_call_function_single() return
		 * value.
		 *
		 * If event->oncpu isn't a valid CPU it means the event got
		 * scheduled out and that will have updated the event count.
		 *
		 * Therefore, either way, we'll have an up-to-date event count
		 * after this.
		 */
		(void)smp_call_function_single(cpu_to_read, __perf_event_read, &data, 1);
		ret = data.ret;
	} else if (event->state == PERF_EVENT_STATE_INACTIVE) {
		struct perf_event_context *ctx = event->ctx;
		unsigned long flags;

		raw_spin_lock_irqsave(&ctx->lock, flags);
		/*
		 * may read while context is not active
		 * (e.g., thread is blocked), in that case
		 * we cannot update context time
		 */
		if (ctx->is_active) {
			update_context_time(ctx);
			update_cgrp_time_from_event(event);
		}
		if (group)
			update_group_times(event);
		else
			update_event_times(event);
		raw_spin_unlock_irqrestore(&ctx->lock, flags);
	}

	return ret;
}
