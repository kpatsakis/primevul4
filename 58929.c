static size_t php_sockop_read(php_stream *stream, char *buf, size_t count)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	ssize_t nr_bytes = 0;
	int err;

	if (!sock || sock->socket == -1) {
		return 0;
	}

	if (sock->is_blocked) {
		php_sock_stream_wait_for_data(stream, sock);
		if (sock->timeout_event)
			return 0;
	}

	nr_bytes = recv(sock->socket, buf, XP_SOCK_BUF_SIZE(count), (sock->is_blocked && sock->timeout.tv_sec != -1) ? MSG_DONTWAIT : 0);
	err = php_socket_errno();

	stream->eof = (nr_bytes == 0 || (nr_bytes == -1 && err != EWOULDBLOCK && err != EAGAIN));

	if (nr_bytes > 0) {
		php_stream_notify_progress_increment(PHP_STREAM_CONTEXT(stream), nr_bytes, 0);
	}

	if (nr_bytes < 0) {
		nr_bytes = 0;
	}

	return nr_bytes;
}
