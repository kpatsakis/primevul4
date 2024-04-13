struct snd_seq_queue *snd_seq_queue_find_name(char *name)
{
	int i;
	struct snd_seq_queue *q;

	for (i = 0; i < SNDRV_SEQ_MAX_QUEUES; i++) {
		if ((q = queueptr(i)) != NULL) {
			if (strncmp(q->name, name, sizeof(q->name)) == 0)
				return q;
			queuefree(q);
		}
	}
	return NULL;
}
