static struct sock *rfcomm_sock_alloc(struct net *net, struct socket *sock, int proto, gfp_t prio)
{
	struct rfcomm_dlc *d;
	struct sock *sk;

	sk = sk_alloc(net, PF_BLUETOOTH, prio, &rfcomm_proto);
	if (!sk)
		return NULL;

	sock_init_data(sock, sk);
	INIT_LIST_HEAD(&bt_sk(sk)->accept_q);

	d = rfcomm_dlc_alloc(prio);
	if (!d) {
		sk_free(sk);
		return NULL;
	}

	d->data_ready   = rfcomm_sk_data_ready;
	d->state_change = rfcomm_sk_state_change;

	rfcomm_pi(sk)->dlc = d;
	d->owner = sk;

	sk->sk_destruct = rfcomm_sock_destruct;
	sk->sk_sndtimeo = RFCOMM_CONN_TIMEOUT;

	sk->sk_sndbuf = RFCOMM_MAX_CREDITS * RFCOMM_DEFAULT_MTU * 10;
	sk->sk_rcvbuf = RFCOMM_MAX_CREDITS * RFCOMM_DEFAULT_MTU * 10;

	sock_reset_flag(sk, SOCK_ZAPPED);

	sk->sk_protocol = proto;
	sk->sk_state    = BT_OPEN;

	bt_sock_link(&rfcomm_sk_list, sk);

	BT_DBG("sk %p", sk);
	return sk;
}
