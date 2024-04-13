static int l2tp_ip_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len)
{
	struct sockaddr_l2tpip *lsa = (struct sockaddr_l2tpip *) uaddr;
	int rc;

	if (sock_flag(sk, SOCK_ZAPPED)) /* Must bind first - autobinding does not work */
		return -EINVAL;

	if (addr_len < sizeof(*lsa))
		return -EINVAL;

	if (ipv4_is_multicast(lsa->l2tp_addr.s_addr))
		return -EINVAL;

	rc = ip4_datagram_connect(sk, uaddr, addr_len);
	if (rc < 0)
		return rc;

	lock_sock(sk);

	l2tp_ip_sk(sk)->peer_conn_id = lsa->l2tp_conn_id;

	write_lock_bh(&l2tp_ip_lock);
	hlist_del_init(&sk->sk_bind_node);
	sk_add_bind_node(sk, &l2tp_ip_bind_table);
	write_unlock_bh(&l2tp_ip_lock);

	release_sock(sk);
	return rc;
}
