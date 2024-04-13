static void ring_buffer_wakeup(struct perf_event *event)
{
	struct ring_buffer *rb;

	rcu_read_lock();
	rb = rcu_dereference(event->rb);
	if (!rb)
		goto unlock;

	list_for_each_entry_rcu(event, &rb->event_list, rb_entry)
		wake_up_all(&event->waitq);

unlock:
	rcu_read_unlock();
}
