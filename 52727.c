int snd_seq_queue_is_used(int queueid, int client)
{
	struct snd_seq_queue *q;
	int result;

	q = queueptr(queueid);
	if (q == NULL)
		return -EINVAL; /* invalid queue */
	result = test_bit(client, q->clients_bitmap) ? 1 : 0;
	queuefree(q);
	return result;
}
