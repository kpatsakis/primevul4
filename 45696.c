static void tpacket_destruct_skb(struct sk_buff *skb)
{
	struct packet_sock *po = pkt_sk(skb->sk);
	void *ph;

	if (likely(po->tx_ring.pg_vec)) {
		__u32 ts;

		ph = skb_shinfo(skb)->destructor_arg;
		BUG_ON(atomic_read(&po->tx_ring.pending) == 0);
		atomic_dec(&po->tx_ring.pending);

		ts = __packet_set_timestamp(po, ph, skb);
		__packet_set_status(po, ph, TP_STATUS_AVAILABLE | ts);
	}

	sock_wfree(skb);
}
