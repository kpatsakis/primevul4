void snd_seq_queue_client_leave_cells(int client)
{
	int i;
	struct snd_seq_queue *q;

	for (i = 0; i < SNDRV_SEQ_MAX_QUEUES; i++) {
		if ((q = queueptr(i)) == NULL)
			continue;
		snd_seq_prioq_leave(q->tickq, client, 0);
		snd_seq_prioq_leave(q->timeq, client, 0);
		queuefree(q);
	}
}
