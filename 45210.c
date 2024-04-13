static int compat_sock_getsockopt(struct socket *sock, int level, int optname,
				char __user *optval, int __user *optlen)
{
	if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO)
		return do_get_sock_timeout(sock, level, optname, optval, optlen);
	return sock_getsockopt(sock, level, optname, optval, optlen);
}
