mm_sshpam_respond(void *ctx, u_int num, char **resp)
{
	Buffer m;
	u_int i;
	int ret;

	debug3("%s", __func__);
	buffer_init(&m);
	buffer_put_int(&m, num);
	for (i = 0; i < num; ++i)
		buffer_put_cstring(&m, resp[i]);
	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_PAM_RESPOND, &m);
	debug3("%s: waiting for MONITOR_ANS_PAM_RESPOND", __func__);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_PAM_RESPOND, &m);
	ret = buffer_get_int(&m);
	debug3("%s: pam_respond returned %d", __func__, ret);
	buffer_free(&m);
	return (ret);
}
