static void __perf_event_period(struct perf_event *event,
				struct perf_cpu_context *cpuctx,
				struct perf_event_context *ctx,
				void *info)
{
	u64 value = *((u64 *)info);
	bool active;

	if (event->attr.freq) {
		event->attr.sample_freq = value;
	} else {
		event->attr.sample_period = value;
		event->hw.sample_period = value;
	}

	active = (event->state == PERF_EVENT_STATE_ACTIVE);
	if (active) {
		perf_pmu_disable(ctx->pmu);
		/*
		 * We could be throttled; unthrottle now to avoid the tick
		 * trying to unthrottle while we already re-started the event.
		 */
		if (event->hw.interrupts == MAX_INTERRUPTS) {
			event->hw.interrupts = 0;
			perf_log_throttle(event, 1);
		}
		event->pmu->stop(event, PERF_EF_UPDATE);
	}

	local64_set(&event->hw.period_left, 0);

	if (active) {
		event->pmu->start(event, PERF_EF_RELOAD);
		perf_pmu_enable(ctx->pmu);
	}
}
