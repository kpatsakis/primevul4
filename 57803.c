static int sctp_v6_available(union sctp_addr *addr, struct sctp_sock *sp)
{
	int type;
	struct net *net = sock_net(&sp->inet.sk);
	const struct in6_addr *in6 = (const struct in6_addr *)&addr->v6.sin6_addr;

	type = ipv6_addr_type(in6);
	if (IPV6_ADDR_ANY == type)
		return 1;
	if (type == IPV6_ADDR_MAPPED) {
		if (sp && ipv6_only_sock(sctp_opt2sk(sp)))
			return 0;
		sctp_v6_map_v4(addr);
		return sctp_get_af_specific(AF_INET)->available(addr, sp);
	}
	if (!(type & IPV6_ADDR_UNICAST))
		return 0;

	return sp->inet.freebind || net->ipv6.sysctl.ip_nonlocal_bind ||
		ipv6_chk_addr(net, in6, NULL, 0);
}
