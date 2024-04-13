mm_auth_rsa_generate_challenge(Key *key)
{
	Buffer m;
	BIGNUM *challenge;
	u_char *blob;
	u_int blen;

	debug3("%s entering", __func__);

	if ((challenge = BN_new()) == NULL)
		fatal("%s: BN_new failed", __func__);

	key->type = KEY_RSA;    /* XXX cheat for key_to_blob */
	if (key_to_blob(key, &blob, &blen) == 0)
		fatal("%s: key_to_blob failed", __func__);
	key->type = KEY_RSA1;

	buffer_init(&m);
	buffer_put_string(&m, blob, blen);
	free(blob);

	mm_request_send(pmonitor->m_recvfd, MONITOR_REQ_RSACHALLENGE, &m);
	mm_request_receive_expect(pmonitor->m_recvfd, MONITOR_ANS_RSACHALLENGE, &m);

	buffer_get_bignum2(&m, challenge);
	buffer_free(&m);

	return (challenge);
}
