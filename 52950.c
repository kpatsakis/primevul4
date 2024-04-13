static int read_next_command(void)
{
	static int stdin_eof = 0;

	if (stdin_eof) {
		unread_command_buf = 0;
		return EOF;
	}

	for (;;) {
		const char *p;

		if (unread_command_buf) {
			unread_command_buf = 0;
		} else {
			struct recent_command *rc;

			strbuf_detach(&command_buf, NULL);
			stdin_eof = strbuf_getline(&command_buf, stdin, '\n');
			if (stdin_eof)
				return EOF;

			if (!seen_data_command
				&& !starts_with(command_buf.buf, "feature ")
				&& !starts_with(command_buf.buf, "option ")) {
				parse_argv();
			}

			rc = rc_free;
			if (rc)
				rc_free = rc->next;
			else {
				rc = cmd_hist.next;
				cmd_hist.next = rc->next;
				cmd_hist.next->prev = &cmd_hist;
				free(rc->buf);
			}

			rc->buf = command_buf.buf;
			rc->prev = cmd_tail;
			rc->next = cmd_hist.prev;
			rc->prev->next = rc;
			cmd_tail = rc;
		}
		if (skip_prefix(command_buf.buf, "get-mark ", &p)) {
			parse_get_mark(p);
			continue;
		}
		if (skip_prefix(command_buf.buf, "cat-blob ", &p)) {
			parse_cat_blob(p);
			continue;
		}
		if (command_buf.buf[0] == '#')
			continue;
		return 0;
	}
}
