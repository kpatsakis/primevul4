static inline int php_tcp_sockop_connect(php_stream *stream, php_netstream_data_t *sock,
		php_stream_xport_param *xparam)
{
	char *host = NULL, *bindto = NULL;
	int portno, bindport = 0;
	int err = 0;
	int ret;
	zval *tmpzval = NULL;
	long sockopts = STREAM_SOCKOP_NONE;

#ifdef AF_UNIX
	if (stream->ops == &php_stream_unix_socket_ops || stream->ops == &php_stream_unixdg_socket_ops) {
		struct sockaddr_un unix_addr;

		sock->socket = socket(PF_UNIX, stream->ops == &php_stream_unix_socket_ops ? SOCK_STREAM : SOCK_DGRAM, 0);

		if (sock->socket == SOCK_ERR) {
			if (xparam->want_errortext) {
				xparam->outputs.error_text = strpprintf(0, "Failed to create unix socket");
			}
			return -1;
		}

		parse_unix_address(xparam, &unix_addr);

		ret = php_network_connect_socket(sock->socket,
				(const struct sockaddr *)&unix_addr, (socklen_t) XtOffsetOf(struct sockaddr_un, sun_path) + xparam->inputs.namelen,
				xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC, xparam->inputs.timeout,
				xparam->want_errortext ? &xparam->outputs.error_text : NULL,
				&err);

		xparam->outputs.error_code = err;

		goto out;
	}
#endif

	host = parse_ip_address(xparam, &portno);

	if (host == NULL) {
		return -1;
	}

	if (PHP_STREAM_CONTEXT(stream) && (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "bindto")) != NULL) {
		if (Z_TYPE_P(tmpzval) != IS_STRING) {
			if (xparam->want_errortext) {
				xparam->outputs.error_text = strpprintf(0, "local_addr context option is not a string.");
			}
			efree(host);
			return -1;
		}
		bindto = parse_ip_address_ex(Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval), &bindport, xparam->want_errortext, &xparam->outputs.error_text);
	}

#ifdef SO_BROADCAST
	if (stream->ops == &php_stream_udp_socket_ops /* SO_BROADCAST is only applicable for UDP */
		&& PHP_STREAM_CONTEXT(stream)
		&& (tmpzval = php_stream_context_get_option(PHP_STREAM_CONTEXT(stream), "socket", "so_broadcast")) != NULL
		&& zend_is_true(tmpzval)
	) {
		sockopts |= STREAM_SOCKOP_SO_BROADCAST;
	}
#endif

	/* Note: the test here for php_stream_udp_socket_ops is important, because we
	 * want the default to be TCP sockets so that the openssl extension can
	 * re-use this code. */

	sock->socket = php_network_connect_socket_to_host(host, portno,
			stream->ops == &php_stream_udp_socket_ops ? SOCK_DGRAM : SOCK_STREAM,
			xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&err,
			bindto,
			bindport,
			sockopts
			);

	ret = sock->socket == -1 ? -1 : 0;
	xparam->outputs.error_code = err;

	if (host) {
		efree(host);
	}
	if (bindto) {
		efree(bindto);
	}

#ifdef AF_UNIX
out:
#endif

	if (ret >= 0 && xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC && err == EINPROGRESS) {
		/* indicates pending connection */
		return 1;
	}

	return ret;
}
