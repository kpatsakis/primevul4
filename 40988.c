void perf_event_task_tick(void)
{
	struct list_head *head = &__get_cpu_var(rotation_list);
	struct perf_cpu_context *cpuctx, *tmp;
	struct perf_event_context *ctx;
	int throttled;

	WARN_ON(!irqs_disabled());

	__this_cpu_inc(perf_throttled_seq);
	throttled = __this_cpu_xchg(perf_throttled_count, 0);

	list_for_each_entry_safe(cpuctx, tmp, head, rotation_list) {
		ctx = &cpuctx->ctx;
		perf_adjust_freq_unthr_context(ctx, throttled);

		ctx = cpuctx->task_ctx;
		if (ctx)
			perf_adjust_freq_unthr_context(ctx, throttled);

		if (cpuctx->jiffies_interval == 1 ||
				!(jiffies % cpuctx->jiffies_interval))
			perf_rotate_context(cpuctx);
	}
}
