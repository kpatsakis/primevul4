int snd_seq_queue_check_access(int queueid, int client)
{
	struct snd_seq_queue *q = queueptr(queueid);
	int access_ok;
	unsigned long flags;

	if (! q)
		return 0;
	spin_lock_irqsave(&q->owner_lock, flags);
	access_ok = check_access(q, client);
	spin_unlock_irqrestore(&q->owner_lock, flags);
	queuefree(q);
	return access_ok;
}
