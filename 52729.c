void snd_seq_queue_remove_cells(int client, struct snd_seq_remove_events *info)
{
	int i;
	struct snd_seq_queue *q;

	for (i = 0; i < SNDRV_SEQ_MAX_QUEUES; i++) {
		if ((q = queueptr(i)) == NULL)
			continue;
		if (test_bit(client, q->clients_bitmap) &&
		    (! (info->remove_mode & SNDRV_SEQ_REMOVE_DEST) ||
		     q->queue == info->queue)) {
			snd_seq_prioq_remove_events(q->tickq, client, info);
			snd_seq_prioq_remove_events(q->timeq, client, info);
		}
		queuefree(q);
	}
}
