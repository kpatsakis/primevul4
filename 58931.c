static size_t php_sockop_write(php_stream *stream, const char *buf, size_t count)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	int didwrite;
	struct timeval *ptimeout;

	if (!sock || sock->socket == -1) {
		return 0;
	}

	if (sock->timeout.tv_sec == -1)
		ptimeout = NULL;
	else
		ptimeout = &sock->timeout;

retry:
	didwrite = send(sock->socket, buf, XP_SOCK_BUF_SIZE(count), (sock->is_blocked && ptimeout) ? MSG_DONTWAIT : 0);

	if (didwrite <= 0) {
		int err = php_socket_errno();
		char *estr;

		if (sock->is_blocked && (err == EWOULDBLOCK || err == EAGAIN)) {
			int retval;

			sock->timeout_event = 0;

			do {
				retval = php_pollfd_for(sock->socket, POLLOUT, ptimeout);

				if (retval == 0) {
					sock->timeout_event = 1;
					break;
				}

				if (retval > 0) {
					/* writable now; retry */
					goto retry;
				}

				err = php_socket_errno();
			} while (err == EINTR);
		}
		estr = php_socket_strerror(err, NULL, 0);
		php_error_docref(NULL, E_NOTICE, "send of " ZEND_LONG_FMT " bytes failed with errno=%ld %s",
				(zend_long)count, err, estr);
		efree(estr);
	}

	if (didwrite > 0) {
		php_stream_notify_progress_increment(PHP_STREAM_CONTEXT(stream), didwrite, 0);
	}

	if (didwrite < 0) {
		didwrite = 0;
	}

	return didwrite;
}
