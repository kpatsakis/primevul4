static inline int php_tcp_sockop_bind(php_stream *stream, php_netstream_data_t *sock,
		php_stream_xport_param *xparam)
{
	char *host = NULL;
	int portno, err;
	long sockopts = STREAM_SOCKOP_NONE;
	zval *tmpzval = NULL;

#ifdef AF_UNIX
	if (stream->ops == &php_stream_unix_socket_ops || stream->ops == &php_stream_unixdg_socket_ops) {
		struct sockaddr_un unix_addr;

		sock->socket = socket(PF_UNIX, stream->ops == &php_stream_unix_socket_ops ? SOCK_STREAM : SOCK_DGRAM, 0);

		if (sock->socket == SOCK_ERR) {
			if (xparam->want_errortext) {
				xparam->outputs.error_text = strpprintf(0, "Failed to create unix%s socket %s",
						stream->ops == &php_stream_unix_socket_ops ? "" : "datagram",
						strerror(errno));
			}
			return -1;
		}

		parse_unix_address(xparam, &unix_addr);

		return bind(sock->socket, (const struct sockaddr *)&unix_addr,
			(socklen_t) XtOffsetOf(struct sockaddr_un, sun_path) + xparam->inputs.namelen);
	}
#endif

	host = parse_ip_address(xparam, &portno);

	if (host == NULL) {
		return -1;
	}

#ifdef IPV6_V6ONLY
	if (PHP_STREAM_CONTEXT(stream)
		&& (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "ipv6_v6only")) != NULL
		&& Z_TYPE_P(tmpzval) != IS_NULL
	) {
		sockopts |= STREAM_SOCKOP_IPV6_V6ONLY;
		sockopts |= STREAM_SOCKOP_IPV6_V6ONLY_ENABLED * zend_is_true(tmpzval);
	}
#endif

#ifdef SO_REUSEPORT
	if (PHP_STREAM_CONTEXT(stream)
		&& (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "so_reuseport")) != NULL
		&& zend_is_true(tmpzval)
	) {
		sockopts |= STREAM_SOCKOP_SO_REUSEPORT;
	}
#endif

#ifdef SO_BROADCAST
	if (stream->ops == &php_stream_udp_socket_ops /* SO_BROADCAST is only applicable for UDP */
		&& PHP_STREAM_CONTEXT(stream)
		&& (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "so_broadcast")) != NULL
		&& zend_is_true(tmpzval)
	) {
		sockopts |= STREAM_SOCKOP_SO_BROADCAST;
	}
#endif

	sock->socket = php_network_bind_socket_to_local_addr(host, portno,
			stream->ops == &php_stream_udp_socket_ops ? SOCK_DGRAM : SOCK_STREAM,
			sockopts,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&err
			);

	if (host) {
		efree(host);
	}

	return sock->socket == -1 ? -1 : 0;
}
