static bool ipv4_datagram_support_cmsg(const struct sock *sk,
				       struct sk_buff *skb,
				       int ee_origin)
{
	struct in_pktinfo *info;

	if (ee_origin == SO_EE_ORIGIN_ICMP)
		return true;

	if (ee_origin == SO_EE_ORIGIN_LOCAL)
		return false;

	/* Support IP_PKTINFO on tstamp packets if requested, to correlate
	 * timestamp with egress dev. Not possible for packets without dev
	 * or without payload (SOF_TIMESTAMPING_OPT_TSONLY).
	 */
	if ((!(sk->sk_tsflags & SOF_TIMESTAMPING_OPT_CMSG)) ||
	    (!skb->dev))
		return false;

	info = PKTINFO_SKB_CB(skb);
	info->ipi_spec_dst.s_addr = ip_hdr(skb)->saddr;
	info->ipi_ifindex = skb->dev->ifindex;
	return true;
}
