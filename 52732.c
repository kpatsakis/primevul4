int snd_seq_queue_timer_open(int queueid)
{
	int result = 0;
	struct snd_seq_queue *queue;
	struct snd_seq_timer *tmr;

	queue = queueptr(queueid);
	if (queue == NULL)
		return -EINVAL;
	tmr = queue->timer;
	if ((result = snd_seq_timer_open(queue)) < 0) {
		snd_seq_timer_defaults(tmr);
		result = snd_seq_timer_open(queue);
	}
	queuefree(queue);
	return result;
}
