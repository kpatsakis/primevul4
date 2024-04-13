asmlinkage long compat_sys_getsockopt(int fd, int level, int optname,
				char __user *optval, int __user *optlen)
{
	int err;
	struct socket *sock = sockfd_lookup(fd, &err);

	if (sock) {
		err = security_socket_getsockopt(sock, level, optname);
		if (err) {
			sockfd_put(sock);
			return err;
		}

		if (level == SOL_SOCKET)
			err = compat_sock_getsockopt(sock, level,
					optname, optval, optlen);
		else if (sock->ops->compat_getsockopt)
			err = sock->ops->compat_getsockopt(sock, level,
					optname, optval, optlen);
		else
			err = sock->ops->getsockopt(sock, level,
					optname, optval, optlen);
		sockfd_put(sock);
	}
	return err;
}
