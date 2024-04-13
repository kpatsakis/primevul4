mm_answer_skeyquery(int sock, Buffer *m)
{
	struct skey skey;
	char challenge[1024];
	u_int success;

	success = _compat_skeychallenge(&skey, authctxt->user, challenge,
	    sizeof(challenge)) < 0 ? 0 : 1;

	buffer_clear(m);
	buffer_put_int(m, success);
	if (success)
		buffer_put_cstring(m, challenge);

	debug3("%s: sending challenge success: %u", __func__, success);
	mm_request_send(sock, MONITOR_ANS_SKEYQUERY, m);

	return (0);
}
