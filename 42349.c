static struct sock *__l2tp_ip6_bind_lookup(struct net *net,
					   struct in6_addr *laddr,
					   int dif, u32 tunnel_id)
{
	struct hlist_node *node;
	struct sock *sk;

	sk_for_each_bound(sk, node, &l2tp_ip6_bind_table) {
		struct in6_addr *addr = inet6_rcv_saddr(sk);
		struct l2tp_ip6_sock *l2tp = l2tp_ip6_sk(sk);

		if (l2tp == NULL)
			continue;

		if ((l2tp->conn_id == tunnel_id) &&
		    net_eq(sock_net(sk), net) &&
		    !(addr && ipv6_addr_equal(addr, laddr)) &&
		    !(sk->sk_bound_dev_if && sk->sk_bound_dev_if != dif))
			goto found;
	}

	sk = NULL;
found:
	return sk;
}
