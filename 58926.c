static void php_sock_stream_wait_for_data(php_stream *stream, php_netstream_data_t *sock)
{
	int retval;
	struct timeval *ptimeout;

	if (!sock || sock->socket == -1) {
		return;
	}

	sock->timeout_event = 0;

	if (sock->timeout.tv_sec == -1)
		ptimeout = NULL;
	else
		ptimeout = &sock->timeout;

	while(1) {
		retval = php_pollfd_for(sock->socket, PHP_POLLREADABLE, ptimeout);

		if (retval == 0)
			sock->timeout_event = 1;

		if (retval >= 0)
			break;

		if (php_socket_errno() != EINTR)
			break;
	}
}
