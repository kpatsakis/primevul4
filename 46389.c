mm_auth_rsa_key_allowed(struct passwd *pw, BIGNUM *client_n, Key **rkey)
{
	Buffer m;
	Key *key;
	u_char *blob;
	u_int blen;
	int allowed = 0, have_forced = 0;

	debug3("%s entering", __func__);

	buffer_init(&m);
	buffer_put_bignum2(&m, client_n);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_RSAKEYALLOWED, &m);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_RSAKEYALLOWED, &m);

	allowed = buffer_get_int(&m);

	/* fake forced command */
	auth_clear_options();
	have_forced = buffer_get_int(&m);
	forced_command = have_forced ? xstrdup("true") : NULL;

	if (allowed && rkey != NULL) {
		blob = buffer_get_string(&m, &blen);
		if ((key = key_from_blob(blob, blen)) == NULL)
			fatal("%s: key_from_blob failed", __func__);
		*rkey = key;
		free(blob);
	}
	buffer_free(&m);

	return (allowed);
}
