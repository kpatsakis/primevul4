int sock_no_sendmsg(struct socket *sock, struct msghdr *m, size_t len)
{
	return -EOPNOTSUPP;
}
