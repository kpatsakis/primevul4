static int queue_list_add(struct snd_seq_queue *q)
{
	int i;
	unsigned long flags;

	spin_lock_irqsave(&queue_list_lock, flags);
	for (i = 0; i < SNDRV_SEQ_MAX_QUEUES; i++) {
		if (! queue_list[i]) {
			queue_list[i] = q;
			q->queue = i;
			num_queues++;
			spin_unlock_irqrestore(&queue_list_lock, flags);
			return i;
		}
	}
	spin_unlock_irqrestore(&queue_list_lock, flags);
	return -1;
}
