static int compat_sock_setsockopt(struct socket *sock, int level, int optname,
				char __user *optval, unsigned int optlen)
{
	if (optname == SO_ATTACH_FILTER)
		return do_set_attach_filter(sock, level, optname,
					    optval, optlen);
	if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO)
		return do_set_sock_timeout(sock, level, optname, optval, optlen);

	return sock_setsockopt(sock, level, optname, optval, optlen);
}
