static int ipx_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	struct sock *sk = sock->sk;
	int rc;

	lock_sock(sk);
	rc = __ipx_bind(sock, uaddr, addr_len);
	release_sock(sk);

	return rc;
}
