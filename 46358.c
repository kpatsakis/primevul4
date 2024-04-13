mm_answer_rsa_response(int sock, Buffer *m)
{
	Key *key = NULL;
	u_char *blob, *response;
	u_int blen, len;
	int success;

	debug3("%s entering", __func__);

	if (!authctxt->valid)
		fatal("%s: authctxt not valid", __func__);
	if (ssh1_challenge == NULL)
		fatal("%s: no ssh1_challenge", __func__);

	blob = buffer_get_string(m, &blen);
	if (!monitor_allowed_key(blob, blen))
		fatal("%s: bad key, not previously allowed", __func__);
	if (key_blobtype != MM_RSAUSERKEY && key_blobtype != MM_RSAHOSTKEY)
		fatal("%s: key type mismatch: %d", __func__, key_blobtype);
	if ((key = key_from_blob(blob, blen)) == NULL)
		fatal("%s: received bad key", __func__);
	response = buffer_get_string(m, &len);
	if (len != 16)
		fatal("%s: received bad response to challenge", __func__);
	success = auth_rsa_verify_response(key, ssh1_challenge, response);

	free(blob);
	key_free(key);
	free(response);

	auth_method = key_blobtype == MM_RSAUSERKEY ? "rsa" : "rhosts-rsa";

	/* reset state */
	BN_clear_free(ssh1_challenge);
	ssh1_challenge = NULL;
	monitor_reset_key_state();

	buffer_clear(m);
	buffer_put_int(m, success);
	mm_request_send(sock, MONITOR_ANS_RSARESPONSE, m);

	return (success);
}
