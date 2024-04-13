static int queue_access_lock(struct snd_seq_queue *q, int client)
{
	unsigned long flags;
	int access_ok;
	
	spin_lock_irqsave(&q->owner_lock, flags);
	access_ok = check_access(q, client);
	if (access_ok)
		q->klocked = 1;
	spin_unlock_irqrestore(&q->owner_lock, flags);
	return access_ok;
}
