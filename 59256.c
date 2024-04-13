static int perf_event_stop(struct perf_event *event, int restart)
{
	struct stop_event_data sd = {
		.event		= event,
		.restart	= restart,
	};
	int ret = 0;

	do {
		if (READ_ONCE(event->state) != PERF_EVENT_STATE_ACTIVE)
			return 0;

		/* matches smp_wmb() in event_sched_in() */
		smp_rmb();

		/*
		 * We only want to restart ACTIVE events, so if the event goes
		 * inactive here (event->oncpu==-1), there's nothing more to do;
		 * fall through with ret==-ENXIO.
		 */
		ret = cpu_function_call(READ_ONCE(event->oncpu),
					__perf_event_stop, &sd);
	} while (ret == -EAGAIN);

	return ret;
}
