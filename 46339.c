mm_answer_authpassword(int sock, Buffer *m)
{
	static int call_count;
	char *passwd;
	int authenticated;
	u_int plen;

	passwd = buffer_get_string(m, &plen);
	/* Only authenticate if the context is valid */
	authenticated = options.password_authentication &&
	    auth_password(authctxt, passwd);
	explicit_bzero(passwd, strlen(passwd));
	free(passwd);

	buffer_clear(m);
	buffer_put_int(m, authenticated);

	debug3("%s: sending result %d", __func__, authenticated);
	mm_request_send(sock, MONITOR_ANS_AUTHPASSWORD, m);

	call_count++;
	if (plen == 0 && call_count == 1)
		auth_method = "none";
	else
		auth_method = "password";

	/* Causes monitor loop to terminate if authenticated */
	return (authenticated);
}
