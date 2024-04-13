static struct sctp_sockaddr_entry *sctp_addr_wq_lookup(struct net *net,
					struct sctp_sockaddr_entry *addr)
{
	struct sctp_sockaddr_entry *addrw;

	list_for_each_entry(addrw, &net->sctp.addr_waitq, list) {
		if (addrw->a.sa.sa_family != addr->a.sa.sa_family)
			continue;
		if (addrw->a.sa.sa_family == AF_INET) {
			if (addrw->a.v4.sin_addr.s_addr ==
			    addr->a.v4.sin_addr.s_addr)
				return addrw;
		} else if (addrw->a.sa.sa_family == AF_INET6) {
			if (ipv6_addr_equal(&addrw->a.v6.sin6_addr,
			    &addr->a.v6.sin6_addr))
				return addrw;
		}
	}
	return NULL;
}
