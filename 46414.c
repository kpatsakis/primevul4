mm_ssh_gssapi_userok(char *user)
{
	Buffer m;
	int authenticated = 0;

	buffer_init(&m);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_GSSUSEROK, &m);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_GSSUSEROK,
				  &m);

	authenticated = buffer_get_int(&m);

	buffer_free(&m);
	debug3("%s: user %sauthenticated",__func__, authenticated ? "" : "not ");
	return (authenticated);
}
