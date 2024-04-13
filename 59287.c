static void put_event(struct perf_event *event)
{
	if (!atomic_long_dec_and_test(&event->refcount))
		return;

	_free_event(event);
}
