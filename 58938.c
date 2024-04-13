static inline int sock_sendto(php_netstream_data_t *sock, const char *buf, size_t buflen, int flags,
		struct sockaddr *addr, socklen_t addrlen
		)
{
	int ret;
	if (addr) {
		ret = sendto(sock->socket, buf, XP_SOCK_BUF_SIZE(buflen), flags, addr, XP_SOCK_BUF_SIZE(addrlen));

		return (ret == SOCK_CONN_ERR) ? -1 : ret;
	}
#ifdef PHP_WIN32
	return ((ret = send(sock->socket, buf, buflen > INT_MAX ? INT_MAX : (int)buflen, flags)) == SOCK_CONN_ERR) ? -1 : ret;
#else
	return ((ret = send(sock->socket, buf, buflen, flags)) == SOCK_CONN_ERR) ? -1 : ret;
#endif
}
