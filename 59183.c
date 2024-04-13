static int __perf_event_stop(void *info)
{
	struct stop_event_data *sd = info;
	struct perf_event *event = sd->event;

	/* if it's already INACTIVE, do nothing */
	if (READ_ONCE(event->state) != PERF_EVENT_STATE_ACTIVE)
		return 0;

	/* matches smp_wmb() in event_sched_in() */
	smp_rmb();

	/*
	 * There is a window with interrupts enabled before we get here,
	 * so we need to check again lest we try to stop another CPU's event.
	 */
	if (READ_ONCE(event->oncpu) != smp_processor_id())
		return -EAGAIN;

	event->pmu->stop(event, PERF_EF_UPDATE);

	/*
	 * May race with the actual stop (through perf_pmu_output_stop()),
	 * but it is only used for events with AUX ring buffer, and such
	 * events will refuse to restart because of rb::aux_mmap_count==0,
	 * see comments in perf_aux_output_begin().
	 *
	 * Since this is happening on a event-local CPU, no trace is lost
	 * while restarting.
	 */
	if (sd->restart)
		event->pmu->start(event, 0);

	return 0;
}
