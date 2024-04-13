void icmpv6_flow_init(struct sock *sk, struct flowi6 *fl6,
		      u8 type,
		      const struct in6_addr *saddr,
		      const struct in6_addr *daddr,
		      int oif)
{
	memset(fl6, 0, sizeof(*fl6));
	fl6->saddr = *saddr;
	fl6->daddr = *daddr;
	fl6->flowi6_proto	= IPPROTO_ICMPV6;
	fl6->fl6_icmp_type	= type;
	fl6->fl6_icmp_code	= 0;
	fl6->flowi6_oif		= oif;
	security_sk_classify_flow(sk, flowi6_to_flowi(fl6));
}
