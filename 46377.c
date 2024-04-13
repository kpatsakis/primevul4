monitor_read_log(struct monitor *pmonitor)
{
	Buffer logmsg;
	u_int len, level;
	char *msg;

	buffer_init(&logmsg);

	/* Read length */
	buffer_append_space(&logmsg, 4);
	if (atomicio(read, pmonitor->m_log_recvfd,
	    buffer_ptr(&logmsg), buffer_len(&logmsg)) != buffer_len(&logmsg)) {
		if (errno == EPIPE) {
			buffer_free(&logmsg);
			debug("%s: child log fd closed", __func__);
			close(pmonitor->m_log_recvfd);
			pmonitor->m_log_recvfd = -1;
			return -1;
		}
		fatal("%s: log fd read: %s", __func__, strerror(errno));
	}
	len = buffer_get_int(&logmsg);
	if (len <= 4 || len > 8192)
		fatal("%s: invalid log message length %u", __func__, len);

	/* Read severity, message */
	buffer_clear(&logmsg);
	buffer_append_space(&logmsg, len);
	if (atomicio(read, pmonitor->m_log_recvfd,
	    buffer_ptr(&logmsg), buffer_len(&logmsg)) != buffer_len(&logmsg))
		fatal("%s: log fd read: %s", __func__, strerror(errno));

	/* Log it */
	level = buffer_get_int(&logmsg);
	msg = buffer_get_string(&logmsg, NULL);
	if (log_level_name(level) == NULL)
		fatal("%s: invalid log level %u (corrupted message?)",
		    __func__, level);
	do_log2(level, "%s [preauth]", msg);

	buffer_free(&logmsg);
	free(msg);

	return 0;
}
