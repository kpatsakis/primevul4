static __inline__ void udpv6_err(struct sk_buff *skb,
				 struct inet6_skb_parm *opt, u8 type,
				 u8 code, int offset, __be32 info)
{
	__udp6_lib_err(skb, opt, type, code, offset, info, &udp_table);
}
