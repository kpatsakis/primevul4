static int pppoe_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct pppox_sock *po;
	struct pppoe_net *pn;
	struct net *net = NULL;

	if (!sk)
		return 0;

	lock_sock(sk);
	if (sock_flag(sk, SOCK_DEAD)) {
		release_sock(sk);
		return -EBADF;
	}

	po = pppox_sk(sk);

	if (sk->sk_state & (PPPOX_CONNECTED | PPPOX_BOUND | PPPOX_ZOMBIE)) {
		dev_put(po->pppoe_dev);
		po->pppoe_dev = NULL;
	}

	pppox_unbind_sock(sk);

	/* Signal the death of the socket. */
	sk->sk_state = PPPOX_DEAD;

	net = sock_net(sk);
	pn = pppoe_pernet(net);

	/*
	 * protect "po" from concurrent updates
	 * on pppoe_flush_dev
	 */
	delete_item(pn, po->pppoe_pa.sid, po->pppoe_pa.remote,
		    po->pppoe_ifindex);

	sock_orphan(sk);
	sock->sk = NULL;

	skb_queue_purge(&sk->sk_receive_queue);
	release_sock(sk);
	sock_put(sk);

	return 0;
}
