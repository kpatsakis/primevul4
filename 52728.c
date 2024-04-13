static void snd_seq_queue_process_event(struct snd_seq_queue *q,
					struct snd_seq_event *ev,
					int atomic, int hop)
{
	switch (ev->type) {
	case SNDRV_SEQ_EVENT_START:
		snd_seq_prioq_leave(q->tickq, ev->source.client, 1);
		snd_seq_prioq_leave(q->timeq, ev->source.client, 1);
		if (! snd_seq_timer_start(q->timer))
			queue_broadcast_event(q, ev, atomic, hop);
		break;

	case SNDRV_SEQ_EVENT_CONTINUE:
		if (! snd_seq_timer_continue(q->timer))
			queue_broadcast_event(q, ev, atomic, hop);
		break;

	case SNDRV_SEQ_EVENT_STOP:
		snd_seq_timer_stop(q->timer);
		queue_broadcast_event(q, ev, atomic, hop);
		break;

	case SNDRV_SEQ_EVENT_TEMPO:
		snd_seq_timer_set_tempo(q->timer, ev->data.queue.param.value);
		queue_broadcast_event(q, ev, atomic, hop);
		break;

	case SNDRV_SEQ_EVENT_SETPOS_TICK:
		if (snd_seq_timer_set_position_tick(q->timer, ev->data.queue.param.time.tick) == 0) {
			queue_broadcast_event(q, ev, atomic, hop);
		}
		break;

	case SNDRV_SEQ_EVENT_SETPOS_TIME:
		if (snd_seq_timer_set_position_time(q->timer, ev->data.queue.param.time.time) == 0) {
			queue_broadcast_event(q, ev, atomic, hop);
		}
		break;
	case SNDRV_SEQ_EVENT_QUEUE_SKEW:
		if (snd_seq_timer_set_skew(q->timer,
					   ev->data.queue.param.skew.value,
					   ev->data.queue.param.skew.base) == 0) {
			queue_broadcast_event(q, ev, atomic, hop);
		}
		break;
	}
}
