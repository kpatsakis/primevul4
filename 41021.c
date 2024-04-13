static void ring_buffer_put(struct ring_buffer *rb)
{
	struct perf_event *event, *n;
	unsigned long flags;

	if (!atomic_dec_and_test(&rb->refcount))
		return;

	spin_lock_irqsave(&rb->event_lock, flags);
	list_for_each_entry_safe(event, n, &rb->event_list, rb_entry) {
		list_del_init(&event->rb_entry);
		wake_up_all(&event->waitq);
	}
	spin_unlock_irqrestore(&rb->event_lock, flags);

	call_rcu(&rb->rcu_head, rb_free_rcu);
}
