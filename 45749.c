void tipc_sock_release_local(struct socket *sock)
{
	release(sock);
	sock->ops = NULL;
	sock_release(sock);
}
