mm_sshpam_free_ctx(void *ctxtp)
{
	Buffer m;

	debug3("%s", __func__);
	buffer_init(&m);
	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_PAM_FREE_CTX, &m);
	debug3("%s: waiting for MONITOR_ANS_PAM_FREE_CTX", __func__);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_PAM_FREE_CTX, &m);
	buffer_free(&m);
}
