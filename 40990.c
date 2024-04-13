void perf_event_wakeup(struct perf_event *event)
{
	ring_buffer_wakeup(event);

	if (event->pending_kill) {
		kill_fasync(&event->fasync, SIGIO, event->pending_kill);
		event->pending_kill = 0;
	}
}
