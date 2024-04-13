static int ipx_shutdown(struct socket *sock, int mode)
{
	struct sock *sk = sock->sk;

	if (mode < SHUT_RD || mode > SHUT_RDWR)
		return -EINVAL;
	/* This maps:
	 * SHUT_RD   (0) -> RCV_SHUTDOWN  (1)
	 * SHUT_WR   (1) -> SEND_SHUTDOWN (2)
	 * SHUT_RDWR (2) -> SHUTDOWN_MASK (3)
	 */
	++mode;

	lock_sock(sk);
	sk->sk_shutdown |= mode;
	release_sock(sk);
	sk->sk_state_change(sk);

	return 0;
}
