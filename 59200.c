event_sched_out(struct perf_event *event,
		  struct perf_cpu_context *cpuctx,
		  struct perf_event_context *ctx)
{
	u64 tstamp = perf_event_time(event);
	u64 delta;

	WARN_ON_ONCE(event->ctx != ctx);
	lockdep_assert_held(&ctx->lock);

	/*
	 * An event which could not be activated because of
	 * filter mismatch still needs to have its timings
	 * maintained, otherwise bogus information is return
	 * via read() for time_enabled, time_running:
	 */
	if (event->state == PERF_EVENT_STATE_INACTIVE &&
	    !event_filter_match(event)) {
		delta = tstamp - event->tstamp_stopped;
		event->tstamp_running += delta;
		event->tstamp_stopped = tstamp;
	}

	if (event->state != PERF_EVENT_STATE_ACTIVE)
		return;

	perf_pmu_disable(event->pmu);

	event->tstamp_stopped = tstamp;
	event->pmu->del(event, 0);
	event->oncpu = -1;
	event->state = PERF_EVENT_STATE_INACTIVE;
	if (event->pending_disable) {
		event->pending_disable = 0;
		event->state = PERF_EVENT_STATE_OFF;
	}

	if (!is_software_event(event))
		cpuctx->active_oncpu--;
	if (!--ctx->nr_active)
		perf_event_ctx_deactivate(ctx);
	if (event->attr.freq && event->attr.sample_freq)
		ctx->nr_freq--;
	if (event->attr.exclusive || !cpuctx->active_oncpu)
		cpuctx->exclusive = 0;

	perf_pmu_enable(event->pmu);
}
