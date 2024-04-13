mm_answer_gss_userok(int sock, Buffer *m)
{
	int authenticated;

	authenticated = authctxt->valid && ssh_gssapi_userok(authctxt->user);

	buffer_clear(m);
	buffer_put_int(m, authenticated);

	debug3("%s: sending result %d", __func__, authenticated);
	mm_request_send(sock, MONITOR_ANS_GSSUSEROK, m);

	auth_method = "gssapi-with-mic";

	/* Monitor loop will terminate if authenticated */
	return (authenticated);
}
