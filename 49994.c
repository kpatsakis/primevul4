static void icmpv6_err(struct sk_buff *skb, struct inet6_skb_parm *opt,
		       u8 type, u8 code, int offset, __be32 info)
{
	/* icmpv6_notify checks 8 bytes can be pulled, icmp6hdr is 8 bytes */
	struct icmp6hdr *icmp6 = (struct icmp6hdr *) (skb->data + offset);
	struct net *net = dev_net(skb->dev);

	if (type == ICMPV6_PKT_TOOBIG)
		ip6_update_pmtu(skb, net, info, 0, 0);
	else if (type == NDISC_REDIRECT)
		ip6_redirect(skb, net, skb->dev->ifindex, 0);

	if (!(type & ICMPV6_INFOMSG_MASK))
		if (icmp6->icmp6_type == ICMPV6_ECHO_REQUEST)
			ping_err(skb, offset, ntohl(info));
}
