int snd_seq_queue_delete(int client, int queueid)
{
	struct snd_seq_queue *q;

	if (queueid < 0 || queueid >= SNDRV_SEQ_MAX_QUEUES)
		return -EINVAL;
	q = queue_list_remove(queueid, client);
	if (q == NULL)
		return -EINVAL;
	queue_delete(q);

	return 0;
}
