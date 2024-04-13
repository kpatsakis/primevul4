mm_get_keystate(struct monitor *pmonitor)
{
	debug3("%s: Waiting for new keys", __func__);

	if ((child_state = sshbuf_new()) == NULL)
		fatal("%s: sshbuf_new failed", __func__);
	mm_request_receive_expect(pmonitor->m_sendfd, MONITOR_REQ_KEYEXPORT,
	    child_state);
	debug3("%s: GOT new keys", __func__);
}
