static int hidp_queue_report(struct hidp_session *session,
				unsigned char *data, int size)
{
	struct sk_buff *skb;

	BT_DBG("session %p hid %p data %p size %d", session, session->hid, data, size);

	skb = alloc_skb(size + 1, GFP_ATOMIC);
	if (!skb) {
		BT_ERR("Can't allocate memory for new frame");
		return -ENOMEM;
	}

	*skb_put(skb, 1) = 0xa2;
	if (size > 0)
		memcpy(skb_put(skb, size), data, size);

	skb_queue_tail(&session->intr_transmit, skb);

	hidp_schedule(session);

	return 0;
}
