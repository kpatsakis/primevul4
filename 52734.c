int snd_seq_queue_use(int queueid, int client, int use)
{
	struct snd_seq_queue *queue;

	queue = queueptr(queueid);
	if (queue == NULL)
		return -EINVAL;
	mutex_lock(&queue->timer_mutex);
	if (use) {
		if (!test_and_set_bit(client, queue->clients_bitmap))
			queue->clients++;
	} else {
		if (test_and_clear_bit(client, queue->clients_bitmap))
			queue->clients--;
	}
	if (queue->clients) {
		if (use && queue->clients == 1)
			snd_seq_timer_defaults(queue->timer);
		snd_seq_timer_open(queue);
	} else {
		snd_seq_timer_close(queue);
	}
	mutex_unlock(&queue->timer_mutex);
	queuefree(queue);
	return 0;
}
