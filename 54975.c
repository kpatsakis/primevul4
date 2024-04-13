send_attrib(u_int32_t id, const Attrib *a)
{
	struct sshbuf *msg;
	int r;

	debug("request %u: sent attrib have 0x%x", id, a->flags);
	if ((msg = sshbuf_new()) == NULL)
		fatal("%s: sshbuf_new failed", __func__);
	if ((r = sshbuf_put_u8(msg, SSH2_FXP_ATTRS)) != 0 ||
	    (r = sshbuf_put_u32(msg, id)) != 0 ||
	    (r = encode_attrib(msg, a)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
	send_msg(msg);
	sshbuf_free(msg);
}
