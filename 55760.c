int tcp_peek_len(struct socket *sock)
{
	return tcp_inq(sock->sk);
}
