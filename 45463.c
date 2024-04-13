static int rfcomm_sock_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	int err = 0;

	BT_DBG("sk %p backlog %d", sk, backlog);

	lock_sock(sk);

	if (sk->sk_state != BT_BOUND) {
		err = -EBADFD;
		goto done;
	}

	if (sk->sk_type != SOCK_STREAM) {
		err = -EINVAL;
		goto done;
	}

	if (!rfcomm_pi(sk)->channel) {
		bdaddr_t *src = &rfcomm_pi(sk)->src;
		u8 channel;

		err = -EINVAL;

		write_lock(&rfcomm_sk_list.lock);

		for (channel = 1; channel < 31; channel++)
			if (!__rfcomm_get_sock_by_addr(channel, src)) {
				rfcomm_pi(sk)->channel = channel;
				err = 0;
				break;
			}

		write_unlock(&rfcomm_sk_list.lock);

		if (err < 0)
			goto done;
	}

	sk->sk_max_ack_backlog = backlog;
	sk->sk_ack_backlog = 0;
	sk->sk_state = BT_LISTEN;

done:
	release_sock(sk);
	return err;
}
