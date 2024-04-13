mm_key_allowed(enum mm_keytype type, char *user, char *host, Key *key,
    int pubkey_auth_attempt)
{
	Buffer m;
	u_char *blob;
	u_int len;
	int allowed = 0, have_forced = 0;

	debug3("%s entering", __func__);

	/* Convert the key to a blob and the pass it over */
	if (!key_to_blob(key, &blob, &len))
		return (0);

	buffer_init(&m);
	buffer_put_int(&m, type);
	buffer_put_cstring(&m, user ? user : "");
	buffer_put_cstring(&m, host ? host : "");
	buffer_put_string(&m, blob, len);
	buffer_put_int(&m, pubkey_auth_attempt);
	free(blob);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_KEYALLOWED, &m);

	debug3("%s: waiting for MONITOR_ANS_KEYALLOWED", __func__);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_KEYALLOWED, &m);

	allowed = buffer_get_int(&m);

	/* fake forced command */
	auth_clear_options();
	have_forced = buffer_get_int(&m);
	forced_command = have_forced ? xstrdup("true") : NULL;

	buffer_free(&m);

	return (allowed);
}
