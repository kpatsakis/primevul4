static unsigned int perf_poll(struct file *file, poll_table *wait)
{
	struct perf_event *event = file->private_data;
	struct ring_buffer *rb;
	unsigned int events = POLL_HUP;

	/*
	 * Race between perf_event_set_output() and perf_poll(): perf_poll()
	 * grabs the rb reference but perf_event_set_output() overrides it.
	 * Here is the timeline for two threads T1, T2:
	 * t0: T1, rb = rcu_dereference(event->rb)
	 * t1: T2, old_rb = event->rb
	 * t2: T2, event->rb = new rb
	 * t3: T2, ring_buffer_detach(old_rb)
	 * t4: T1, ring_buffer_attach(rb1)
	 * t5: T1, poll_wait(event->waitq)
	 *
	 * To avoid this problem, we grab mmap_mutex in perf_poll()
	 * thereby ensuring that the assignment of the new ring buffer
	 * and the detachment of the old buffer appear atomic to perf_poll()
	 */
	mutex_lock(&event->mmap_mutex);

	rcu_read_lock();
	rb = rcu_dereference(event->rb);
	if (rb) {
		ring_buffer_attach(event, rb);
		events = atomic_xchg(&rb->poll, 0);
	}
	rcu_read_unlock();

	mutex_unlock(&event->mmap_mutex);

	poll_wait(file, &event->waitq, wait);

	return events;
}
