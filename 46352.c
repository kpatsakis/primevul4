mm_answer_pam_respond(int sock, Buffer *m)
{
	char **resp;
	u_int i, num;
	int ret;

	debug3("%s", __func__);
	sshpam_authok = NULL;
	num = buffer_get_int(m);
	if (num > 0) {
		resp = xcalloc(num, sizeof(char *));
		for (i = 0; i < num; ++i)
			resp[i] = buffer_get_string(m, NULL);
		ret = (sshpam_device.respond)(sshpam_ctxt, num, resp);
		for (i = 0; i < num; ++i)
			free(resp[i]);
		free(resp);
	} else {
		ret = (sshpam_device.respond)(sshpam_ctxt, num, NULL);
	}
	buffer_clear(m);
	buffer_put_int(m, ret);
	mm_request_send(sock, MONITOR_ANS_PAM_RESPOND, m);
	auth_method = "keyboard-interactive";
	auth_submethod = "pam";
	if (ret == 0)
		sshpam_authok = sshpam_ctxt;
	return (0);
}
