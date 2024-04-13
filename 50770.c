static void tcp_v6_fill_cb(struct sk_buff *skb, const struct ipv6hdr *hdr,
			   const struct tcphdr *th)
{
	/* This is tricky: we move IP6CB at its correct location into
	 * TCP_SKB_CB(). It must be done after xfrm6_policy_check(), because
	 * _decode_session6() uses IP6CB().
	 * barrier() makes sure compiler won't play aliasing games.
	 */
	memmove(&TCP_SKB_CB(skb)->header.h6, IP6CB(skb),
		sizeof(struct inet6_skb_parm));
	barrier();

	TCP_SKB_CB(skb)->seq = ntohl(th->seq);
	TCP_SKB_CB(skb)->end_seq = (TCP_SKB_CB(skb)->seq + th->syn + th->fin +
				    skb->len - th->doff*4);
	TCP_SKB_CB(skb)->ack_seq = ntohl(th->ack_seq);
	TCP_SKB_CB(skb)->tcp_flags = tcp_flag_byte(th);
	TCP_SKB_CB(skb)->tcp_tw_isn = 0;
	TCP_SKB_CB(skb)->ip_dsfield = ipv6_get_dsfield(hdr);
	TCP_SKB_CB(skb)->sacked = 0;
}
