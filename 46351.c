mm_answer_pam_query(int sock, Buffer *m)
{
	char *name = NULL, *info = NULL, **prompts = NULL;
	u_int i, num = 0, *echo_on = 0;
	int ret;

	debug3("%s", __func__);
	sshpam_authok = NULL;
	ret = (sshpam_device.query)(sshpam_ctxt, &name, &info, &num, &prompts, &echo_on);
	if (ret == 0 && num == 0)
		sshpam_authok = sshpam_ctxt;
	if (num > 1 || name == NULL || info == NULL)
		ret = -1;
	buffer_clear(m);
	buffer_put_int(m, ret);
	buffer_put_cstring(m, name);
	free(name);
	buffer_put_cstring(m, info);
	free(info);
	buffer_put_int(m, num);
	for (i = 0; i < num; ++i) {
		buffer_put_cstring(m, prompts[i]);
		free(prompts[i]);
		buffer_put_int(m, echo_on[i]);
	}
	free(prompts);
	free(echo_on);
	auth_method = "keyboard-interactive";
	auth_submethod = "pam";
	mm_request_send(sock, MONITOR_ANS_PAM_QUERY, m);
	return (0);
}
