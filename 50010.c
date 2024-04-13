static bool is_ineligible(const struct sk_buff *skb)
{
	int ptr = (u8 *)(ipv6_hdr(skb) + 1) - skb->data;
	int len = skb->len - ptr;
	__u8 nexthdr = ipv6_hdr(skb)->nexthdr;
	__be16 frag_off;

	if (len < 0)
		return true;

	ptr = ipv6_skip_exthdr(skb, ptr, &nexthdr, &frag_off);
	if (ptr < 0)
		return false;
	if (nexthdr == IPPROTO_ICMPV6) {
		u8 _type, *tp;
		tp = skb_header_pointer(skb,
			ptr+offsetof(struct icmp6hdr, icmp6_type),
			sizeof(_type), &_type);
		if (!tp || !(*tp & ICMPV6_INFOMSG_MASK))
			return true;
	}
	return false;
}
