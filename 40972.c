static inline void perf_event__state_init(struct perf_event *event)
{
	event->state = event->attr.disabled ? PERF_EVENT_STATE_OFF :
					      PERF_EVENT_STATE_INACTIVE;
}
