static inline int sock_recvfrom(php_netstream_data_t *sock, char *buf, size_t buflen, int flags,
		zend_string **textaddr,
		struct sockaddr **addr, socklen_t *addrlen
		)
{
	int ret;
	int want_addr = textaddr || addr;

	if (want_addr) {
		php_sockaddr_storage sa;
		socklen_t sl = sizeof(sa);
		ret = recvfrom(sock->socket, buf, XP_SOCK_BUF_SIZE(buflen), flags, (struct sockaddr*)&sa, &sl);
		ret = (ret == SOCK_CONN_ERR) ? -1 : ret;
		if (sl) {
			php_network_populate_name_from_sockaddr((struct sockaddr*)&sa, sl,
					textaddr, addr, addrlen);
		} else {
			if (textaddr) {
				*textaddr = ZSTR_EMPTY_ALLOC();
			}
			if (addr) {
				*addr = NULL;
				*addrlen = 0;
			}
		}
	} else {
		ret = recv(sock->socket, buf, XP_SOCK_BUF_SIZE(buflen), flags);
		ret = (ret == SOCK_CONN_ERR) ? -1 : ret;
	}

	return ret;
}
