int snd_seq_queue_timer_close(int queueid)
{
	struct snd_seq_queue *queue;
	int result = 0;

	queue = queueptr(queueid);
	if (queue == NULL)
		return -EINVAL;
	snd_seq_timer_close(queue);
	queuefree(queue);
	return result;
}
