static void __perf_event_output_stop(struct perf_event *event, void *data)
{
	struct perf_event *parent = event->parent;
	struct remote_output *ro = data;
	struct ring_buffer *rb = ro->rb;
	struct stop_event_data sd = {
		.event	= event,
	};

	if (!has_aux(event))
		return;

	if (!parent)
		parent = event;

	/*
	 * In case of inheritance, it will be the parent that links to the
	 * ring-buffer, but it will be the child that's actually using it.
	 *
	 * We are using event::rb to determine if the event should be stopped,
	 * however this may race with ring_buffer_attach() (through set_output),
	 * which will make us skip the event that actually needs to be stopped.
	 * So ring_buffer_attach() has to stop an aux event before re-assigning
	 * its rb pointer.
	 */
	if (rcu_dereference(parent->rb) == rb)
		ro->err = __perf_event_stop(&sd);
}
