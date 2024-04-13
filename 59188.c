static void _perf_event_disable(struct perf_event *event)
{
	struct perf_event_context *ctx = event->ctx;

	raw_spin_lock_irq(&ctx->lock);
	if (event->state <= PERF_EVENT_STATE_OFF) {
		raw_spin_unlock_irq(&ctx->lock);
		return;
	}
	raw_spin_unlock_irq(&ctx->lock);

	event_function_call(event, __perf_event_disable, NULL);
}
