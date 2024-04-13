void udp_v4_early_demux(struct sk_buff *skb)
{
	struct net *net = dev_net(skb->dev);
	const struct iphdr *iph;
	const struct udphdr *uh;
	struct sock *sk;
	struct dst_entry *dst;
	int dif = skb->dev->ifindex;

	/* validate the packet */
	if (!pskb_may_pull(skb, skb_transport_offset(skb) + sizeof(struct udphdr)))
		return;

	iph = ip_hdr(skb);
	uh = udp_hdr(skb);

	if (skb->pkt_type == PACKET_BROADCAST ||
	    skb->pkt_type == PACKET_MULTICAST)
		sk = __udp4_lib_mcast_demux_lookup(net, uh->dest, iph->daddr,
						   uh->source, iph->saddr, dif);
	else if (skb->pkt_type == PACKET_HOST)
		sk = __udp4_lib_demux_lookup(net, uh->dest, iph->daddr,
					     uh->source, iph->saddr, dif);
	else
		return;

	if (!sk)
		return;

	skb->sk = sk;
	skb->destructor = sock_efree;
	dst = sk->sk_rx_dst;

	if (dst)
		dst = dst_check(dst, 0);
	if (dst)
		skb_dst_set_noref(skb, dst);
}
