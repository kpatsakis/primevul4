static inline void queue_access_unlock(struct snd_seq_queue *q)
{
	unsigned long flags;

	spin_lock_irqsave(&q->owner_lock, flags);
	q->klocked = 0;
	spin_unlock_irqrestore(&q->owner_lock, flags);
}
