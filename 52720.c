void snd_seq_info_queues_read(struct snd_info_entry *entry, 
			      struct snd_info_buffer *buffer)
{
	int i, bpm;
	struct snd_seq_queue *q;
	struct snd_seq_timer *tmr;

	for (i = 0; i < SNDRV_SEQ_MAX_QUEUES; i++) {
		if ((q = queueptr(i)) == NULL)
			continue;

		tmr = q->timer;
		if (tmr->tempo)
			bpm = 60000000 / tmr->tempo;
		else
			bpm = 0;

		snd_iprintf(buffer, "queue %d: [%s]\n", q->queue, q->name);
		snd_iprintf(buffer, "owned by client    : %d\n", q->owner);
		snd_iprintf(buffer, "lock status        : %s\n", q->locked ? "Locked" : "Free");
		snd_iprintf(buffer, "queued time events : %d\n", snd_seq_prioq_avail(q->timeq));
		snd_iprintf(buffer, "queued tick events : %d\n", snd_seq_prioq_avail(q->tickq));
		snd_iprintf(buffer, "timer state        : %s\n", tmr->running ? "Running" : "Stopped");
		snd_iprintf(buffer, "timer PPQ          : %d\n", tmr->ppq);
		snd_iprintf(buffer, "current tempo      : %d\n", tmr->tempo);
		snd_iprintf(buffer, "current BPM        : %d\n", bpm);
		snd_iprintf(buffer, "current time       : %d.%09d s\n", tmr->cur_time.tv_sec, tmr->cur_time.tv_nsec);
		snd_iprintf(buffer, "current tick       : %d\n", tmr->tick.cur_tick);
		snd_iprintf(buffer, "\n");
		queuefree(q);
	}
}
