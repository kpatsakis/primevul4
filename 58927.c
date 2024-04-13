static int php_sockop_cast(php_stream *stream, int castas, void **ret)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	if (!sock) {
		return FAILURE;
	}

	switch(castas)	{
		case PHP_STREAM_AS_STDIO:
			if (ret)	{
				*(FILE**)ret = fdopen(sock->socket, stream->mode);
				if (*ret)
					return SUCCESS;
				return FAILURE;
			}
			return SUCCESS;
		case PHP_STREAM_AS_FD_FOR_SELECT:
		case PHP_STREAM_AS_FD:
		case PHP_STREAM_AS_SOCKETD:
			if (ret)
				*(php_socket_t *)ret = sock->socket;
			return SUCCESS;
		default:
			return FAILURE;
	}
}
