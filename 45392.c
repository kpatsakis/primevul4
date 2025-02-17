static int atalk_create(struct net *net, struct socket *sock, int protocol,
			int kern)
{
	struct sock *sk;
	int rc = -ESOCKTNOSUPPORT;

	if (!net_eq(net, &init_net))
		return -EAFNOSUPPORT;

	/*
	 * We permit SOCK_DGRAM and RAW is an extension. It is trivial to do
	 * and gives you the full ELAP frame. Should be handy for CAP 8)
	 */
	if (sock->type != SOCK_RAW && sock->type != SOCK_DGRAM)
		goto out;
	rc = -ENOMEM;
	sk = sk_alloc(net, PF_APPLETALK, GFP_KERNEL, &ddp_proto);
	if (!sk)
		goto out;
	rc = 0;
	sock->ops = &atalk_dgram_ops;
	sock_init_data(sock, sk);

	/* Checksums on by default */
	sock_set_flag(sk, SOCK_ZAPPED);
out:
	return rc;
}
