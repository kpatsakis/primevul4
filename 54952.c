process(void)
{
	u_int msg_len;
	u_int buf_len;
	u_int consumed;
	u_char type;
	const u_char *cp;
	int i, r;
	u_int32_t id;

	buf_len = sshbuf_len(iqueue);
	if (buf_len < 5)
		return;		/* Incomplete message. */
	cp = sshbuf_ptr(iqueue);
	msg_len = get_u32(cp);
	if (msg_len > SFTP_MAX_MSG_LENGTH) {
		error("bad message from %s local user %s",
		    client_addr, pw->pw_name);
		sftp_server_cleanup_exit(11);
	}
	if (buf_len < msg_len + 4)
		return;
	if ((r = sshbuf_consume(iqueue, 4)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
	buf_len -= 4;
	if ((r = sshbuf_get_u8(iqueue, &type)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	switch (type) {
	case SSH2_FXP_INIT:
		process_init();
		init_done = 1;
		break;
	case SSH2_FXP_EXTENDED:
		if (!init_done)
			fatal("Received extended request before init");
		if ((r = sshbuf_get_u32(iqueue, &id)) != 0)
			fatal("%s: buffer error: %s", __func__, ssh_err(r));
		process_extended(id);
		break;
	default:
		if (!init_done)
			fatal("Received %u request before init", type);
		if ((r = sshbuf_get_u32(iqueue, &id)) != 0)
			fatal("%s: buffer error: %s", __func__, ssh_err(r));
		for (i = 0; handlers[i].handler != NULL; i++) {
			if (type == handlers[i].type) {
				if (!request_permitted(&handlers[i])) {
					send_status(id,
					    SSH2_FX_PERMISSION_DENIED);
				} else {
					handlers[i].handler(id);
				}
				break;
			}
		}
		if (handlers[i].handler == NULL)
			error("Unknown message %u", type);
	}
	/* discard the remaining bytes from the current packet */
	if (buf_len < sshbuf_len(iqueue)) {
		error("iqueue grew unexpectedly");
		sftp_server_cleanup_exit(255);
	}
	consumed = buf_len - sshbuf_len(iqueue);
	if (msg_len < consumed) {
		error("msg_len %u < consumed %u", msg_len, consumed);
		sftp_server_cleanup_exit(255);
	}
	if (msg_len > consumed &&
	    (r = sshbuf_consume(iqueue, msg_len - consumed)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
}
