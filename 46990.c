static int sctp_v4_available(union sctp_addr *addr, struct sctp_sock *sp)
{
	struct net *net = sock_net(&sp->inet.sk);
	int ret = inet_addr_type(net, addr->v4.sin_addr.s_addr);


	if (addr->v4.sin_addr.s_addr != htonl(INADDR_ANY) &&
	   ret != RTN_LOCAL &&
	   !sp->inet.freebind &&
	   !net->ipv4.sysctl_ip_nonlocal_bind)
		return 0;

	if (ipv6_only_sock(sctp_opt2sk(sp)))
		return 0;

	return 1;
}
