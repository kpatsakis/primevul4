static void ip_multipath_l3_keys(const struct sk_buff *skb,
				 struct flow_keys *hash_keys)
{
	const struct iphdr *outer_iph = ip_hdr(skb);
	const struct iphdr *inner_iph;
	const struct icmphdr *icmph;
	struct iphdr _inner_iph;
	struct icmphdr _icmph;

	hash_keys->addrs.v4addrs.src = outer_iph->saddr;
	hash_keys->addrs.v4addrs.dst = outer_iph->daddr;
	if (likely(outer_iph->protocol != IPPROTO_ICMP))
		return;

	if (unlikely((outer_iph->frag_off & htons(IP_OFFSET)) != 0))
		return;

	icmph = skb_header_pointer(skb, outer_iph->ihl * 4, sizeof(_icmph),
				   &_icmph);
	if (!icmph)
		return;

	if (icmph->type != ICMP_DEST_UNREACH &&
	    icmph->type != ICMP_REDIRECT &&
	    icmph->type != ICMP_TIME_EXCEEDED &&
	    icmph->type != ICMP_PARAMETERPROB)
		return;

	inner_iph = skb_header_pointer(skb,
				       outer_iph->ihl * 4 + sizeof(_icmph),
				       sizeof(_inner_iph), &_inner_iph);
	if (!inner_iph)
		return;
	hash_keys->addrs.v4addrs.src = inner_iph->saddr;
	hash_keys->addrs.v4addrs.dst = inner_iph->daddr;
}
