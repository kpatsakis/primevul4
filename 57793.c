static u32 tcp_v6_init_ts_off(const struct sk_buff *skb)
{
	return secure_tcpv6_ts_off(ipv6_hdr(skb)->daddr.s6_addr32,
				   ipv6_hdr(skb)->saddr.s6_addr32);
}
