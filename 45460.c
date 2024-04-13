static int rfcomm_sock_connect(struct socket *sock, struct sockaddr *addr, int alen, int flags)
{
	struct sockaddr_rc *sa = (struct sockaddr_rc *) addr;
	struct sock *sk = sock->sk;
	struct rfcomm_dlc *d = rfcomm_pi(sk)->dlc;
	int err = 0;

	BT_DBG("sk %p", sk);

	if (alen < sizeof(struct sockaddr_rc) ||
	    addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	lock_sock(sk);

	if (sk->sk_state != BT_OPEN && sk->sk_state != BT_BOUND) {
		err = -EBADFD;
		goto done;
	}

	if (sk->sk_type != SOCK_STREAM) {
		err = -EINVAL;
		goto done;
	}

	sk->sk_state = BT_CONNECT;
	bacpy(&rfcomm_pi(sk)->dst, &sa->rc_bdaddr);
	rfcomm_pi(sk)->channel = sa->rc_channel;

	d->sec_level = rfcomm_pi(sk)->sec_level;
	d->role_switch = rfcomm_pi(sk)->role_switch;

	err = rfcomm_dlc_open(d, &rfcomm_pi(sk)->src, &sa->rc_bdaddr,
			      sa->rc_channel);
	if (!err)
		err = bt_sock_wait_state(sk, BT_CONNECTED,
				sock_sndtimeo(sk, flags & O_NONBLOCK));

done:
	release_sock(sk);
	return err;
}
