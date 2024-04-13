mm_send_keystate(struct monitor *monitor)
{
	struct ssh *ssh = active_state;		/* XXX */
	struct sshbuf *m;
	int r;

	if ((m = sshbuf_new()) == NULL)
		fatal("%s: sshbuf_new failed", __func__);
	if ((r = ssh_packet_get_state(ssh, m)) != 0)
		fatal("%s: get_state failed: %s",
		    __func__, ssh_err(r));
	mm_request_send(monitor->m_recvfd, MONITOR_REQ_KEYEXPORT, m);
	debug3("%s: Finished sending state", __func__);
	sshbuf_free(m);
}
