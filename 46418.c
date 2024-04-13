mm_start_pam(Authctxt *authctxt)
{
	Buffer m;

	debug3("%s entering", __func__);
	if (!options.use_pam)
		fatal("UsePAM=no, but ended up in %s anyway", __func__);

	buffer_init(&m);
	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_PAM_START, &m);

	buffer_free(&m);
}
