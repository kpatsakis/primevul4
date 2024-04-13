static int atalk_release(struct socket *sock)
{
	struct sock *sk = sock->sk;

	if (sk) {
		sock_hold(sk);
		lock_sock(sk);

		sock_orphan(sk);
		sock->sk = NULL;
		atalk_destroy_socket(sk);

		release_sock(sk);
		sock_put(sk);
	}
	return 0;
}
