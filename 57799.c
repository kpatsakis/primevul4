static int sctp_inet6_bind_verify(struct sctp_sock *opt, union sctp_addr *addr)
{
	struct sctp_af *af;

	/* ASSERT: address family has already been verified. */
	if (addr->sa.sa_family != AF_INET6)
		af = sctp_get_af_specific(addr->sa.sa_family);
	else {
		int type = ipv6_addr_type(&addr->v6.sin6_addr);
		struct net_device *dev;

		if (type & IPV6_ADDR_LINKLOCAL) {
			struct net *net;
			if (!addr->v6.sin6_scope_id)
				return 0;
			net = sock_net(&opt->inet.sk);
			rcu_read_lock();
			dev = dev_get_by_index_rcu(net, addr->v6.sin6_scope_id);
			if (!dev ||
			    !ipv6_chk_addr(net, &addr->v6.sin6_addr, dev, 0)) {
				rcu_read_unlock();
				return 0;
			}
			rcu_read_unlock();
		}

		af = opt->pf->af;
	}
	return af->available(addr, opt);
}
