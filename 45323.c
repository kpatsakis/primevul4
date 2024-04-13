static struct sock *__l2tp_ip_bind_lookup(struct net *net, __be32 laddr, int dif, u32 tunnel_id)
{
	struct sock *sk;

	sk_for_each_bound(sk, &l2tp_ip_bind_table) {
		struct inet_sock *inet = inet_sk(sk);
		struct l2tp_ip_sock *l2tp = l2tp_ip_sk(sk);

		if (l2tp == NULL)
			continue;

		if ((l2tp->conn_id == tunnel_id) &&
		    net_eq(sock_net(sk), net) &&
		    !(inet->inet_rcv_saddr && inet->inet_rcv_saddr != laddr) &&
		    !(sk->sk_bound_dev_if && sk->sk_bound_dev_if != dif))
			goto found;
	}

	sk = NULL;
found:
	return sk;
}
