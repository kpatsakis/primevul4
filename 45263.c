static int icmp_filter(const struct sock *sk, const struct sk_buff *skb)
{
	struct icmphdr _hdr;
	const struct icmphdr *hdr;

	hdr = skb_header_pointer(skb, skb_transport_offset(skb),
				 sizeof(_hdr), &_hdr);
	if (!hdr)
		return 1;

	if (hdr->type < 32) {
		__u32 data = raw_sk(sk)->filter.data;

		return ((1U << hdr->type) & data) != 0;
	}

	/* Do not block unknown ICMP types */
	return 0;
}
