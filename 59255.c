perf_event_set_output(struct perf_event *event, struct perf_event *output_event)
{
	struct ring_buffer *rb = NULL;
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

	/*
	 * Mixing clocks in the same buffer is trouble you don't need.
	 */
	if (output_event->clock != event->clock)
		goto out;

	/*
	 * Either writing ring buffer from beginning or from end.
	 * Mixing is not allowed.
	 */
	if (is_write_backward(output_event) != is_write_backward(event))
		goto out;

	/*
	 * If both events generate aux data, they must be on the same PMU
	 */
	if (has_aux(event) && has_aux(output_event) &&
	    event->pmu != output_event->pmu)
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

	ring_buffer_attach(event, rb);

	ret = 0;
unlock:
	mutex_unlock(&event->mmap_mutex);

out:
	return ret;
}
