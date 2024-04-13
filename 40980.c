static int perf_event_idx_default(struct perf_event *event)
{
	return event->hw.idx + 1;
}
