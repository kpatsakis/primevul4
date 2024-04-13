void perf_event_disable_inatomic(struct perf_event *event)
{
	event->pending_disable = 1;
	irq_work_queue(&event->pending);
}
