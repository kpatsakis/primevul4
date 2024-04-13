static inline int sock_recvmsg_nosec(struct socket *sock, struct msghdr *msg,
				     int flags)
{
	return sock->ops->recvmsg(sock, msg, msg_data_left(msg), flags);
}
