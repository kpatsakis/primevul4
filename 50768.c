static void tcp_v6_early_demux(struct sk_buff *skb)
{
	const struct ipv6hdr *hdr;
	const struct tcphdr *th;
	struct sock *sk;

	if (skb->pkt_type != PACKET_HOST)
		return;

	if (!pskb_may_pull(skb, skb_transport_offset(skb) + sizeof(struct tcphdr)))
		return;

	hdr = ipv6_hdr(skb);
	th = tcp_hdr(skb);

	if (th->doff < sizeof(struct tcphdr) / 4)
		return;

	/* Note : We use inet6_iif() here, not tcp_v6_iif() */
	sk = __inet6_lookup_established(dev_net(skb->dev), &tcp_hashinfo,
					&hdr->saddr, th->source,
					&hdr->daddr, ntohs(th->dest),
					inet6_iif(skb));
	if (sk) {
		skb->sk = sk;
		skb->destructor = sock_edemux;
		if (sk_fullsock(sk)) {
			struct dst_entry *dst = READ_ONCE(sk->sk_rx_dst);

			if (dst)
				dst = dst_check(dst, inet6_sk(sk)->rx_dst_cookie);
			if (dst &&
			    inet_sk(sk)->rx_dst_ifindex == skb->skb_iif)
				skb_dst_set_noref(skb, dst);
		}
	}
}
