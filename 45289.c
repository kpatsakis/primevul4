static int icmpv6_filter(const struct sock *sk, const struct sk_buff *skb)
{
	struct icmp6hdr _hdr;
	const struct icmp6hdr *hdr;

	/* We require only the four bytes of the ICMPv6 header, not any
	 * additional bytes of message body in "struct icmp6hdr".
	 */
	hdr = skb_header_pointer(skb, skb_transport_offset(skb),
				 ICMPV6_HDRLEN, &_hdr);
	if (hdr) {
		const __u32 *data = &raw6_sk(sk)->filter.data[0];
		unsigned int type = hdr->icmp6_type;

		return (data[type >> 5] & (1U << (type & 31))) != 0;
	}
	return 1;
}
