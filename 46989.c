static int sctp_v4_addr_valid(union sctp_addr *addr,
			      struct sctp_sock *sp,
			      const struct sk_buff *skb)
{
	/* IPv4 addresses not allowed */
	if (sp && ipv6_only_sock(sctp_opt2sk(sp)))
		return 0;

	/* Is this a non-unicast address or a unusable SCTP address? */
	if (IS_IPV4_UNUSABLE_ADDRESS(addr->v4.sin_addr.s_addr))
		return 0;

	/* Is this a broadcast address? */
	if (skb && skb_rtable(skb)->rt_flags & RTCF_BROADCAST)
		return 0;

	return 1;
}
