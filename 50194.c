int sock_no_recvmsg(struct socket *sock, struct msghdr *m, size_t len,
		    int flags)
{
	return -EOPNOTSUPP;
}
