static int atalk_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	struct sockaddr_at *addr = (struct sockaddr_at *)uaddr;
	struct sock *sk = sock->sk;
	struct atalk_sock *at = at_sk(sk);
	int err;

	if (!sock_flag(sk, SOCK_ZAPPED) ||
	    addr_len != sizeof(struct sockaddr_at))
		return -EINVAL;

	if (addr->sat_family != AF_APPLETALK)
		return -EAFNOSUPPORT;

	lock_sock(sk);
	if (addr->sat_addr.s_net == htons(ATADDR_ANYNET)) {
		struct atalk_addr *ap = atalk_find_primary();

		err = -EADDRNOTAVAIL;
		if (!ap)
			goto out;

		at->src_net  = addr->sat_addr.s_net = ap->s_net;
		at->src_node = addr->sat_addr.s_node= ap->s_node;
	} else {
		err = -EADDRNOTAVAIL;
		if (!atalk_find_interface(addr->sat_addr.s_net,
					  addr->sat_addr.s_node))
			goto out;

		at->src_net  = addr->sat_addr.s_net;
		at->src_node = addr->sat_addr.s_node;
	}

	if (addr->sat_port == ATADDR_ANYPORT) {
		err = atalk_pick_and_bind_port(sk, addr);

		if (err < 0)
			goto out;
	} else {
		at->src_port = addr->sat_port;

		err = -EADDRINUSE;
		if (atalk_find_or_insert_socket(sk, addr))
			goto out;
	}

	sock_reset_flag(sk, SOCK_ZAPPED);
	err = 0;
out:
	release_sock(sk);
	return err;
}
