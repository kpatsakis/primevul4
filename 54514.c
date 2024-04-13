struct sock *__raw_v4_lookup(struct net *net, struct sock *sk,
			     unsigned short num, __be32 raddr, __be32 laddr,
			     int dif, int sdif)
{
	sk_for_each_from(sk) {
		struct inet_sock *inet = inet_sk(sk);

		if (net_eq(sock_net(sk), net) && inet->inet_num == num	&&
		    !(inet->inet_daddr && inet->inet_daddr != raddr) 	&&
		    !(inet->inet_rcv_saddr && inet->inet_rcv_saddr != laddr) &&
		    !(sk->sk_bound_dev_if && sk->sk_bound_dev_if != dif &&
		      sk->sk_bound_dev_if != sdif))
			goto found; /* gotcha */
	}
	sk = NULL;
found:
	return sk;
}
