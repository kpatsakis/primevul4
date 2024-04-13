static void irda_connect_response(struct irda_sock *self)
{
	struct sk_buff *skb;

	IRDA_DEBUG(2, "%s()\n", __func__);

	skb = alloc_skb(TTP_MAX_HEADER + TTP_SAR_HEADER, GFP_KERNEL);
	if (skb == NULL) {
		IRDA_DEBUG(0, "%s() Unable to allocate sk_buff!\n",
			   __func__);
		return;
	}

	/* Reserve space for MUX_CONTROL and LAP header */
	skb_reserve(skb, IRDA_MAX_HEADER);

	irttp_connect_response(self->tsap, self->max_sdu_size_rx, skb);
}
