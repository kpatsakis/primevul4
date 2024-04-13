mm_request_receive(int sock, Buffer *m)
{
	u_char buf[4];
	u_int msg_len;

	debug3("%s entering", __func__);

	if (atomicio(read, sock, buf, sizeof(buf)) != sizeof(buf)) {
		if (errno == EPIPE)
			cleanup_exit(255);
		fatal("%s: read: %s", __func__, strerror(errno));
	}
	msg_len = get_u32(buf);
	if (msg_len > 256 * 1024)
		fatal("%s: read: bad msg_len %d", __func__, msg_len);
	buffer_clear(m);
	buffer_append_space(m, msg_len);
	if (atomicio(read, sock, buffer_ptr(m), msg_len) != msg_len)
		fatal("%s: read: %s", __func__, strerror(errno));
}
