static struct sock *__raw_v6_lookup(struct net *net, struct sock *sk,
		unsigned short num, const struct in6_addr *loc_addr,
		const struct in6_addr *rmt_addr, int dif)
{
	bool is_multicast = ipv6_addr_is_multicast(loc_addr);

	sk_for_each_from(sk)
		if (inet_sk(sk)->inet_num == num) {

			if (!net_eq(sock_net(sk), net))
				continue;

			if (!ipv6_addr_any(&sk->sk_v6_daddr) &&
			    !ipv6_addr_equal(&sk->sk_v6_daddr, rmt_addr))
				continue;

			if (sk->sk_bound_dev_if && sk->sk_bound_dev_if != dif)
				continue;

			if (!ipv6_addr_any(&sk->sk_v6_rcv_saddr)) {
				if (ipv6_addr_equal(&sk->sk_v6_rcv_saddr, loc_addr))
					goto found;
				if (is_multicast &&
				    inet6_mc_check(sk, loc_addr, rmt_addr))
					goto found;
				continue;
			}
			goto found;
		}
	sk = NULL;
found:
	return sk;
}
