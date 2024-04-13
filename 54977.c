send_data_or_handle(char type, u_int32_t id, const u_char *data, int dlen)
{
	struct sshbuf *msg;
	int r;

	if ((msg = sshbuf_new()) == NULL)
		fatal("%s: sshbuf_new failed", __func__);
	if ((r = sshbuf_put_u8(msg, type)) != 0 ||
	    (r = sshbuf_put_u32(msg, id)) != 0 ||
	    (r = sshbuf_put_string(msg, data, dlen)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
	send_msg(msg);
	sshbuf_free(msg);
}
