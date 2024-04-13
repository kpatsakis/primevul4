perf_event_set_output(struct perf_event *event, struct perf_event *output_event)
{
	struct ring_buffer *rb = NULL, *old_rb = NULL;
	int ret = -EINVAL;

	if (!output_event)
		goto set;

	/* don't allow circular references */
	if (event == output_event)
		goto out;

	/*
	 * Don't allow cross-cpu buffers
	 */
	if (output_event->cpu != event->cpu)
		goto out;

	/*
	 * If its not a per-cpu rb, it must be the same task.
	 */
	if (output_event->cpu == -1 && output_event->ctx != event->ctx)
		goto out;

set:
	mutex_lock(&event->mmap_mutex);
	/* Can't redirect output if we've got an active mmap() */
	if (atomic_read(&event->mmap_count))
		goto unlock;

	if (output_event) {
		/* get the rb we want to redirect to */
		rb = ring_buffer_get(output_event);
		if (!rb)
			goto unlock;
	}

	old_rb = event->rb;
	rcu_assign_pointer(event->rb, rb);
	if (old_rb)
		ring_buffer_detach(event, old_rb);
	ret = 0;
unlock:
	mutex_unlock(&event->mmap_mutex);

	if (old_rb)
		ring_buffer_put(old_rb);
out:
	return ret;
}
