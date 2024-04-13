int sock_recvmsg(struct socket *sock, struct msghdr *msg, size_t size,
		 int flags)
{
	int err = security_socket_recvmsg(sock, msg, size, flags);

	return err ?: sock_recvmsg_nosec(sock, msg, size, flags);
}
