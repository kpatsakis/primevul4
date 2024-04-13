static inline int compute_score(struct sock *sk, struct net *net,
				__be32 saddr, unsigned short hnum, __be16 sport,
				__be32 daddr, __be16 dport, int dif)
{
	int score;
	struct inet_sock *inet;

	if (!net_eq(sock_net(sk), net) ||
	    udp_sk(sk)->udp_port_hash != hnum ||
	    ipv6_only_sock(sk))
		return -1;

	score = (sk->sk_family == PF_INET) ? 2 : 1;
	inet = inet_sk(sk);

	if (inet->inet_rcv_saddr) {
		if (inet->inet_rcv_saddr != daddr)
			return -1;
		score += 4;
	}

	if (inet->inet_daddr) {
		if (inet->inet_daddr != saddr)
			return -1;
		score += 4;
	}

	if (inet->inet_dport) {
		if (inet->inet_dport != sport)
			return -1;
		score += 4;
	}

	if (sk->sk_bound_dev_if) {
		if (sk->sk_bound_dev_if != dif)
			return -1;
		score += 4;
	}

	return score;
}
