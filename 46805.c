static inline int compute_score2(struct sock *sk, struct net *net,
				 const struct in6_addr *saddr, __be16 sport,
				 const struct in6_addr *daddr,
				 unsigned short hnum, int dif)
{
	int score;
	struct inet_sock *inet;

	if (!net_eq(sock_net(sk), net) ||
	    udp_sk(sk)->udp_port_hash != hnum ||
	    sk->sk_family != PF_INET6)
		return -1;

	if (!ipv6_addr_equal(&sk->sk_v6_rcv_saddr, daddr))
		return -1;

	score = 0;
	inet = inet_sk(sk);

	if (inet->inet_dport) {
		if (inet->inet_dport != sport)
			return -1;
		score++;
	}

	if (!ipv6_addr_any(&sk->sk_v6_daddr)) {
		if (!ipv6_addr_equal(&sk->sk_v6_daddr, saddr))
			return -1;
		score++;
	}

	if (sk->sk_bound_dev_if) {
		if (sk->sk_bound_dev_if != dif)
			return -1;
		score++;
	}

	return score;
}
