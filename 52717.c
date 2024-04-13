struct snd_seq_queue *queueptr(int queueid)
{
	struct snd_seq_queue *q;
	unsigned long flags;

	if (queueid < 0 || queueid >= SNDRV_SEQ_MAX_QUEUES)
		return NULL;
	spin_lock_irqsave(&queue_list_lock, flags);
	q = queue_list[queueid];
	if (q)
		snd_use_lock_use(&q->use_lock);
	spin_unlock_irqrestore(&queue_list_lock, flags);
	return q;
}
