static int afiucv_hs_callback_rx(struct sock *sk, struct sk_buff *skb)
{
	struct iucv_sock *iucv = iucv_sk(sk);

	if (!iucv) {
		kfree_skb(skb);
		return NET_RX_SUCCESS;
	}

	if (sk->sk_state != IUCV_CONNECTED) {
		kfree_skb(skb);
		return NET_RX_SUCCESS;
	}

	if (sk->sk_shutdown & RCV_SHUTDOWN) {
		kfree_skb(skb);
		return NET_RX_SUCCESS;
	}

		/* write stuff from iucv_msg to skb cb */
	if (skb->len < sizeof(struct af_iucv_trans_hdr)) {
		kfree_skb(skb);
		return NET_RX_SUCCESS;
	}
	skb_pull(skb, sizeof(struct af_iucv_trans_hdr));
	skb_reset_transport_header(skb);
	skb_reset_network_header(skb);
	IUCV_SKB_CB(skb)->offset = 0;
	spin_lock(&iucv->message_q.lock);
	if (skb_queue_empty(&iucv->backlog_skb_q)) {
		if (sock_queue_rcv_skb(sk, skb)) {
			/* handle rcv queue full */
			skb_queue_tail(&iucv->backlog_skb_q, skb);
		}
	} else
		skb_queue_tail(&iucv_sk(sk)->backlog_skb_q, skb);
	spin_unlock(&iucv->message_q.lock);
	return NET_RX_SUCCESS;
}
