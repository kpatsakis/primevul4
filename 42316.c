static int tun_release(struct socket *sock)
{
	if (sock->sk)
		sock_put(sock->sk);
	return 0;
}
