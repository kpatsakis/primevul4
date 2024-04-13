PHPAPI php_stream *php_stream_generic_socket_factory(const char *proto, size_t protolen,
		const char *resourcename, size_t resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		php_stream_context *context STREAMS_DC)
{
	php_stream *stream = NULL;
	php_netstream_data_t *sock;
	php_stream_ops *ops;

	/* which type of socket ? */
	if (strncmp(proto, "tcp", protolen) == 0) {
		ops = &php_stream_socket_ops;
	} else if (strncmp(proto, "udp", protolen) == 0) {
		ops = &php_stream_udp_socket_ops;
	}
#ifdef AF_UNIX
	else if (strncmp(proto, "unix", protolen) == 0) {
		ops = &php_stream_unix_socket_ops;
	} else if (strncmp(proto, "udg", protolen) == 0) {
		ops = &php_stream_unixdg_socket_ops;
	}
#endif
	else {
		/* should never happen */
		return NULL;
	}

	sock = pemalloc(sizeof(php_netstream_data_t), persistent_id ? 1 : 0);
	memset(sock, 0, sizeof(php_netstream_data_t));

	sock->is_blocked = 1;
	sock->timeout.tv_sec = FG(default_socket_timeout);
	sock->timeout.tv_usec = 0;

	/* we don't know the socket until we have determined if we are binding or
	 * connecting */
	sock->socket = -1;

	stream = php_stream_alloc_rel(ops, sock, persistent_id, "r+");

	if (stream == NULL)	{
		pefree(sock, persistent_id ? 1 : 0);
		return NULL;
	}

	if (flags == 0) {
		return stream;
	}

	return stream;
}
