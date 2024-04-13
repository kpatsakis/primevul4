static unsigned int br_nf_pre_routing_ipv6(unsigned int hook,
					   struct sk_buff *skb,
					   const struct net_device *in,
					   const struct net_device *out,
					   int (*okfn)(struct sk_buff *))
{
	struct ipv6hdr *hdr;
	u32 pkt_len;

	if (skb->len < sizeof(struct ipv6hdr))
		return NF_DROP;

	if (!pskb_may_pull(skb, sizeof(struct ipv6hdr)))
		return NF_DROP;

	hdr = ipv6_hdr(skb);

	if (hdr->version != 6)
		return NF_DROP;

	pkt_len = ntohs(hdr->payload_len);

	if (pkt_len || hdr->nexthdr != NEXTHDR_HOP) {
		if (pkt_len + sizeof(struct ipv6hdr) > skb->len)
			return NF_DROP;
		if (pskb_trim_rcsum(skb, pkt_len + sizeof(struct ipv6hdr)))
			return NF_DROP;
	}
	if (hdr->nexthdr == NEXTHDR_HOP && check_hbh_len(skb))
		return NF_DROP;

	nf_bridge_put(skb->nf_bridge);
	if (!nf_bridge_alloc(skb))
		return NF_DROP;
	if (!setup_pre_routing(skb))
		return NF_DROP;

	skb->protocol = htons(ETH_P_IPV6);
	NF_HOOK(NFPROTO_IPV6, NF_INET_PRE_ROUTING, skb, skb->dev, NULL,
		br_nf_pre_routing_finish_ipv6);

	return NF_STOLEN;
}
