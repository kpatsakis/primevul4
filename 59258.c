void perf_event_task_tick(void)
{
	struct list_head *head = this_cpu_ptr(&active_ctx_list);
	struct perf_event_context *ctx, *tmp;
	int throttled;

	WARN_ON(!irqs_disabled());

	__this_cpu_inc(perf_throttled_seq);
	throttled = __this_cpu_xchg(perf_throttled_count, 0);
	tick_dep_clear_cpu(smp_processor_id(), TICK_DEP_BIT_PERF_EVENTS);

	list_for_each_entry_safe(ctx, tmp, head, active_ctx_list)
		perf_adjust_freq_unthr_context(ctx, throttled);
}
