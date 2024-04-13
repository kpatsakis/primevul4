static u32 tcp_v6_init_seq(const struct sk_buff *skb)
{
	return secure_tcpv6_seq(ipv6_hdr(skb)->daddr.s6_addr32,
				ipv6_hdr(skb)->saddr.s6_addr32,
				tcp_hdr(skb)->dest,
				tcp_hdr(skb)->source);
}
