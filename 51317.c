static inline int sock_recvmsg_nosec(struct socket *sock, struct msghdr *msg,
				     size_t size, int flags)
{
	return sock->ops->recvmsg(sock, msg, size, flags);
}
