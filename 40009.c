static int sco_sock_connect(struct socket *sock, struct sockaddr *addr, int alen, int flags)
{
	struct sockaddr_sco *sa = (struct sockaddr_sco *) addr;
	struct sock *sk = sock->sk;
	int err = 0;


	BT_DBG("sk %p", sk);

	if (alen < sizeof(struct sockaddr_sco) ||
	    addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	if (sk->sk_state != BT_OPEN && sk->sk_state != BT_BOUND)
		return -EBADFD;

	if (sk->sk_type != SOCK_SEQPACKET)
		return -EINVAL;

	lock_sock(sk);

	/* Set destination address and psm */
	bacpy(&bt_sk(sk)->dst, &sa->sco_bdaddr);

	err = sco_connect(sk);
	if (err)
		goto done;

	err = bt_sock_wait_state(sk, BT_CONNECTED,
				 sock_sndtimeo(sk, flags & O_NONBLOCK));

done:
	release_sock(sk);
	return err;
}
