static struct tcp_md5sig_key *tcp_v6_md5_do_lookup(const struct sock *sk,
						   const struct in6_addr *addr)
{
	return tcp_md5_do_lookup(sk, (union tcp_md5_addr *)addr, AF_INET6);
}
