static struct snd_seq_queue *queue_list_remove(int id, int client)
{
	struct snd_seq_queue *q;
	unsigned long flags;

	spin_lock_irqsave(&queue_list_lock, flags);
	q = queue_list[id];
	if (q) {
		spin_lock(&q->owner_lock);
		if (q->owner == client) {
			/* found */
			q->klocked = 1;
			spin_unlock(&q->owner_lock);
			queue_list[id] = NULL;
			num_queues--;
			spin_unlock_irqrestore(&queue_list_lock, flags);
			return q;
		}
		spin_unlock(&q->owner_lock);
	}
	spin_unlock_irqrestore(&queue_list_lock, flags);
	return NULL;
}
