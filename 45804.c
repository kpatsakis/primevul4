static int x25_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct x25_sock *x25;

	if (!sk)
		return 0;

	x25 = x25_sk(sk);

	sock_hold(sk);
	lock_sock(sk);
	switch (x25->state) {

		case X25_STATE_0:
		case X25_STATE_2:
			x25_disconnect(sk, 0, 0, 0);
			__x25_destroy_socket(sk);
			goto out;

		case X25_STATE_1:
		case X25_STATE_3:
		case X25_STATE_4:
			x25_clear_queues(sk);
			x25_write_internal(sk, X25_CLEAR_REQUEST);
			x25_start_t23timer(sk);
			x25->state = X25_STATE_2;
			sk->sk_state	= TCP_CLOSE;
			sk->sk_shutdown	|= SEND_SHUTDOWN;
			sk->sk_state_change(sk);
			sock_set_flag(sk, SOCK_DEAD);
			sock_set_flag(sk, SOCK_DESTROY);
			break;
	}

	sock_orphan(sk);
out:
	release_sock(sk);
	sock_put(sk);
	return 0;
}
