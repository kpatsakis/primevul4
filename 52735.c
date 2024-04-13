void __exit snd_seq_queues_delete(void)
{
	int i;

	/* clear list */
	for (i = 0; i < SNDRV_SEQ_MAX_QUEUES; i++) {
		if (queue_list[i])
			queue_delete(queue_list[i]);
	}
}
