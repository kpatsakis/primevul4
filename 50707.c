static void tpacket_destruct_skb(struct sk_buff *skb)
{
	struct packet_sock *po = pkt_sk(skb->sk);

	if (likely(po->tx_ring.pg_vec)) {
		void *ph;
		__u32 ts;

		ph = skb_shinfo(skb)->destructor_arg;
		packet_dec_pending(&po->tx_ring);

		ts = __packet_set_timestamp(po, ph, skb);
		__packet_set_status(po, ph, TP_STATUS_AVAILABLE | ts);
	}

	sock_wfree(skb);
}
