void iriap_send_ack(struct iriap_cb *self)
{
	struct sk_buff *tx_skb;
	__u8 *frame;

	IRDA_DEBUG(2, "%s()\n", __func__);

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);

	tx_skb = alloc_skb(LMP_MAX_HEADER + 1, GFP_ATOMIC);
	if (!tx_skb)
		return;

	/* Reserve space for MUX and LAP header */
	skb_reserve(tx_skb, self->max_header_size);
	skb_put(tx_skb, 1);
	frame = tx_skb->data;

	/* Build frame */
	frame[0] = IAP_LST | IAP_ACK | self->operation;

	irlmp_data_request(self->lsap, tx_skb);
}
