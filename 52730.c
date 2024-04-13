int snd_seq_queue_set_owner(int queueid, int client, int locked)
{
	struct snd_seq_queue *q = queueptr(queueid);

	if (q == NULL)
		return -EINVAL;

	if (! queue_access_lock(q, client)) {
		queuefree(q);
		return -EPERM;
	}

	q->locked = locked ? 1 : 0;
	q->owner = client;
	queue_access_unlock(q);
	queuefree(q);

	return 0;
}
