asmlinkage long compat_sys_setsockopt(int fd, int level, int optname,
				char __user *optval, unsigned int optlen)
{
	int err;
	struct socket *sock = sockfd_lookup(fd, &err);

	if (sock) {
		err = security_socket_setsockopt(sock, level, optname);
		if (err) {
			sockfd_put(sock);
			return err;
		}

		if (level == SOL_SOCKET)
			err = compat_sock_setsockopt(sock, level,
					optname, optval, optlen);
		else if (sock->ops->compat_setsockopt)
			err = sock->ops->compat_setsockopt(sock, level,
					optname, optval, optlen);
		else
			err = sock->ops->setsockopt(sock, level,
					optname, optval, optlen);
		sockfd_put(sock);
	}
	return err;
}
