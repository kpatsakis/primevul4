int snd_seq_control_queue(struct snd_seq_event *ev, int atomic, int hop)
{
	struct snd_seq_queue *q;

	if (snd_BUG_ON(!ev))
		return -EINVAL;
	q = queueptr(ev->data.queue.queue);

	if (q == NULL)
		return -EINVAL;

	if (! queue_access_lock(q, ev->source.client)) {
		queuefree(q);
		return -EPERM;
	}

	snd_seq_queue_process_event(q, ev, atomic, hop);

	queue_access_unlock(q);
	queuefree(q);
	return 0;
}
