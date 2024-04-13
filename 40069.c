ax25_cb *ax25_create_cb(void)
{
	ax25_cb *ax25;

	if ((ax25 = kzalloc(sizeof(*ax25), GFP_ATOMIC)) == NULL)
		return NULL;

	atomic_set(&ax25->refcount, 1);

	skb_queue_head_init(&ax25->write_queue);
	skb_queue_head_init(&ax25->frag_queue);
	skb_queue_head_init(&ax25->ack_queue);
	skb_queue_head_init(&ax25->reseq_queue);

	ax25_setup_timers(ax25);

	ax25_fillin_cb(ax25, NULL);

	ax25->state = AX25_STATE_0;

	return ax25;
}
