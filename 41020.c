static void ring_buffer_attach(struct perf_event *event,
			       struct ring_buffer *rb)
{
	unsigned long flags;

	if (!list_empty(&event->rb_entry))
		return;

	spin_lock_irqsave(&rb->event_lock, flags);
	if (!list_empty(&event->rb_entry))
		goto unlock;

	list_add(&event->rb_entry, &rb->event_list);
unlock:
	spin_unlock_irqrestore(&rb->event_lock, flags);
}
