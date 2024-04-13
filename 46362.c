mm_answer_skeyrespond(int sock, Buffer *m)
{
	char *response;
	int authok;

	response = buffer_get_string(m, NULL);

	authok = (options.challenge_response_authentication &&
	    authctxt->valid &&
	    skey_haskey(authctxt->pw->pw_name) == 0 &&
	    skey_passcheck(authctxt->pw->pw_name, response) != -1);

	free(response);

	buffer_clear(m);
	buffer_put_int(m, authok);

	debug3("%s: sending authenticated: %d", __func__, authok);
	mm_request_send(sock, MONITOR_ANS_SKEYRESPOND, m);

	auth_method = "skey";

	return (authok != 0);
}
