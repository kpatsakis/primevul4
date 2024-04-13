int inet_rcv_saddr_equal(const struct sock *sk, const struct sock *sk2,
			 bool match_wildcard)
{
#if IS_ENABLED(CONFIG_IPV6)
	if (sk->sk_family == AF_INET6)
		return ipv6_rcv_saddr_equal(&sk->sk_v6_rcv_saddr,
					    inet6_rcv_saddr(sk2),
					    sk->sk_rcv_saddr,
					    sk2->sk_rcv_saddr,
					    ipv6_only_sock(sk),
					    ipv6_only_sock(sk2),
					    match_wildcard);
#endif
	return ipv4_rcv_saddr_equal(sk->sk_rcv_saddr, sk2->sk_rcv_saddr,
				    ipv6_only_sock(sk2), match_wildcard);
}
