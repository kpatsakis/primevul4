static int iucv_fragment_skb(struct sock *sk, struct sk_buff *skb, int len)
{
	int dataleft, size, copied = 0;
	struct sk_buff *nskb;

	dataleft = len;
	while (dataleft) {
		if (dataleft >= sk->sk_rcvbuf / 4)
			size = sk->sk_rcvbuf / 4;
		else
			size = dataleft;

		nskb = alloc_skb(size, GFP_ATOMIC | GFP_DMA);
		if (!nskb)
			return -ENOMEM;

		/* copy target class to control buffer of new skb */
		IUCV_SKB_CB(nskb)->class = IUCV_SKB_CB(skb)->class;

		/* copy data fragment */
		memcpy(nskb->data, skb->data + copied, size);
		copied += size;
		dataleft -= size;

		skb_reset_transport_header(nskb);
		skb_reset_network_header(nskb);
		nskb->len = size;

		skb_queue_tail(&iucv_sk(sk)->backlog_skb_q, nskb);
	}

	return 0;
}
