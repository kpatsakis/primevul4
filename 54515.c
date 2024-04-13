int raw_abort(struct sock *sk, int err)
{
	lock_sock(sk);

	sk->sk_err = err;
	sk->sk_error_report(sk);
	__udp_disconnect(sk, 0);

	release_sock(sk);

	return 0;
}
