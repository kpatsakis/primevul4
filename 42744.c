static void iriap_disconnect_request(struct iriap_cb *self)
{
	struct sk_buff *tx_skb;

	IRDA_DEBUG(4, "%s()\n", __func__);

	IRDA_ASSERT(self != NULL, return;);
	IRDA_ASSERT(self->magic == IAS_MAGIC, return;);

	tx_skb = alloc_skb(LMP_MAX_HEADER, GFP_ATOMIC);
	if (tx_skb == NULL) {
		IRDA_DEBUG(0,
			   "%s(), Could not allocate an sk_buff of length %d\n",
			   __func__, LMP_MAX_HEADER);
		return;
	}

	/*
	 *  Reserve space for MUX control and LAP header
	 */
	skb_reserve(tx_skb, LMP_MAX_HEADER);

	irlmp_disconnect_request(self->lsap, tx_skb);
}
