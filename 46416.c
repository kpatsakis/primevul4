mm_sshpam_query(void *ctx, char **name, char **info,
    u_int *num, char ***prompts, u_int **echo_on)
{
	Buffer m;
	u_int i;
	int ret;

	debug3("%s", __func__);
	buffer_init(&m);
	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_PAM_QUERY, &m);
	debug3("%s: waiting for MONITOR_ANS_PAM_QUERY", __func__);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_PAM_QUERY, &m);
	ret = buffer_get_int(&m);
	debug3("%s: pam_query returned %d", __func__, ret);
	*name = buffer_get_string(&m, NULL);
	*info = buffer_get_string(&m, NULL);
	*num = buffer_get_int(&m);
	if (*num > PAM_MAX_NUM_MSG)
		fatal("%s: recieved %u PAM messages, expected <= %u",
		    __func__, *num, PAM_MAX_NUM_MSG);
	*prompts = xcalloc((*num + 1), sizeof(char *));
	*echo_on = xcalloc((*num + 1), sizeof(u_int));
	for (i = 0; i < *num; ++i) {
		(*prompts)[i] = buffer_get_string(&m, NULL);
		(*echo_on)[i] = buffer_get_int(&m);
	}
	buffer_free(&m);
	return (ret);
}
