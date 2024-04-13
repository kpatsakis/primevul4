static void ip_cmsg_recv_pktinfo(struct msghdr *msg, struct sk_buff *skb)
{
	struct in_pktinfo info = *PKTINFO_SKB_CB(skb);

	info.ipi_addr.s_addr = ip_hdr(skb)->daddr;

	put_cmsg(msg, SOL_IP, IP_PKTINFO, sizeof(info), &info);
}
