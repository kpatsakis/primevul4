static void queue_broadcast_event(struct snd_seq_queue *q, struct snd_seq_event *ev,
				  int atomic, int hop)
{
	struct snd_seq_event sev;

	sev = *ev;
	
	sev.flags = SNDRV_SEQ_TIME_STAMP_TICK|SNDRV_SEQ_TIME_MODE_ABS;
	sev.time.tick = q->timer->tick.cur_tick;
	sev.queue = q->queue;
	sev.data.queue.queue = q->queue;

	/* broadcast events from Timer port */
	sev.source.client = SNDRV_SEQ_CLIENT_SYSTEM;
	sev.source.port = SNDRV_SEQ_PORT_SYSTEM_TIMER;
	sev.dest.client = SNDRV_SEQ_ADDRESS_SUBSCRIBERS;
	snd_seq_kernel_client_dispatch(SNDRV_SEQ_CLIENT_SYSTEM, &sev, atomic, hop);
}
