static void udp6_csum_zero_error(struct sk_buff *skb)
{
	/* RFC 2460 section 8.1 says that we SHOULD log
	 * this error. Well, it is reasonable.
	 */
	net_dbg_ratelimited("IPv6: udp checksum is 0 for [%pI6c]:%u->[%pI6c]:%u\n",
			    &ipv6_hdr(skb)->saddr, ntohs(udp_hdr(skb)->source),
			    &ipv6_hdr(skb)->daddr, ntohs(udp_hdr(skb)->dest));
}
