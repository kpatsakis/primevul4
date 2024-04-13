static int __hidp_send_ctrl_message(struct hidp_session *session,
				    unsigned char hdr, unsigned char *data,
				    int size)
{
	struct sk_buff *skb;

	BT_DBG("session %p data %p size %d", session, data, size);

	if (atomic_read(&session->terminate))
		return -EIO;

	skb = alloc_skb(size + 1, GFP_ATOMIC);
	if (!skb) {
		BT_ERR("Can't allocate memory for new frame");
		return -ENOMEM;
	}

	*skb_put(skb, 1) = hdr;
	if (data && size > 0)
		memcpy(skb_put(skb, size), data, size);

	skb_queue_tail(&session->ctrl_transmit, skb);

	return 0;
}
