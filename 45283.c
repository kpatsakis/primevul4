void udp_v4_early_demux(struct sk_buff *skb)
{
	const struct iphdr *iph = ip_hdr(skb);
	const struct udphdr *uh = udp_hdr(skb);
	struct sock *sk;
	struct dst_entry *dst;
	struct net *net = dev_net(skb->dev);
	int dif = skb->dev->ifindex;

	/* validate the packet */
	if (!pskb_may_pull(skb, skb_transport_offset(skb) + sizeof(struct udphdr)))
		return;

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
	skb->destructor = sock_edemux;
	dst = sk->sk_rx_dst;

	if (dst)
		dst = dst_check(dst, 0);
	if (dst)
		skb_dst_set_noref(skb, dst);
}
