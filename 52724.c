void snd_seq_queue_client_termination(int client)
{
	unsigned long flags;
	int i;
	struct snd_seq_queue *q;

	for (i = 0; i < SNDRV_SEQ_MAX_QUEUES; i++) {
		if ((q = queueptr(i)) == NULL)
			continue;
		spin_lock_irqsave(&q->owner_lock, flags);
		if (q->owner == client)
			q->klocked = 1;
		spin_unlock_irqrestore(&q->owner_lock, flags);
		if (q->owner == client) {
			if (q->timer->running)
				snd_seq_timer_stop(q->timer);
			snd_seq_timer_reset(q->timer);
		}
		queuefree(q);
	}
}
