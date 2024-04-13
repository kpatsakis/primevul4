mm_answer_keyallowed(int sock, Buffer *m)
{
	Key *key;
	char *cuser, *chost;
	u_char *blob;
	u_int bloblen, pubkey_auth_attempt;
	enum mm_keytype type = 0;
	int allowed = 0;

	debug3("%s entering", __func__);

	type = buffer_get_int(m);
	cuser = buffer_get_string(m, NULL);
	chost = buffer_get_string(m, NULL);
	blob = buffer_get_string(m, &bloblen);
	pubkey_auth_attempt = buffer_get_int(m);

	key = key_from_blob(blob, bloblen);

	if ((compat20 && type == MM_RSAHOSTKEY) ||
	    (!compat20 && type != MM_RSAHOSTKEY))
		fatal("%s: key type and protocol mismatch", __func__);

	debug3("%s: key_from_blob: %p", __func__, key);

	if (key != NULL && authctxt->valid) {
		/* These should not make it past the privsep child */
		if (key_type_plain(key->type) == KEY_RSA &&
		    (datafellows & SSH_BUG_RSASIGMD5) != 0)
			fatal("%s: passed a SSH_BUG_RSASIGMD5 key", __func__);

		switch (type) {
		case MM_USERKEY:
			allowed = options.pubkey_authentication &&
			    !auth2_userkey_already_used(authctxt, key) &&
			    match_pattern_list(sshkey_ssh_name(key),
			    options.pubkey_key_types, 0) == 1 &&
			    user_key_allowed(authctxt->pw, key,
			    pubkey_auth_attempt);
			pubkey_auth_info(authctxt, key, NULL);
			auth_method = "publickey";
			if (options.pubkey_authentication &&
			    (!pubkey_auth_attempt || allowed != 1))
				auth_clear_options();
			break;
		case MM_HOSTKEY:
			allowed = options.hostbased_authentication &&
			    match_pattern_list(sshkey_ssh_name(key),
			    options.hostbased_key_types, 0) == 1 &&
			    hostbased_key_allowed(authctxt->pw,
			    cuser, chost, key);
			pubkey_auth_info(authctxt, key,
			    "client user \"%.100s\", client host \"%.100s\"",
			    cuser, chost);
			auth_method = "hostbased";
			break;
#ifdef WITH_SSH1
		case MM_RSAHOSTKEY:
			key->type = KEY_RSA1; /* XXX */
			allowed = options.rhosts_rsa_authentication &&
			    auth_rhosts_rsa_key_allowed(authctxt->pw,
			    cuser, chost, key);
			if (options.rhosts_rsa_authentication && allowed != 1)
				auth_clear_options();
			auth_method = "rsa";
			break;
#endif
		default:
			fatal("%s: unknown key type %d", __func__, type);
			break;
		}
	}
	if (key != NULL)
		key_free(key);

	/* clear temporarily storage (used by verify) */
	monitor_reset_key_state();

	if (allowed) {
		/* Save temporarily for comparison in verify */
		key_blob = blob;
		key_bloblen = bloblen;
		key_blobtype = type;
		hostbased_cuser = cuser;
		hostbased_chost = chost;
	} else {
		/* Log failed attempt */
		auth_log(authctxt, 0, 0, auth_method, NULL);
		free(blob);
		free(cuser);
		free(chost);
	}

	debug3("%s: key %p is %s",
	    __func__, key, allowed ? "allowed" : "not allowed");

	buffer_clear(m);
	buffer_put_int(m, allowed);
	buffer_put_int(m, forced_command != NULL);

	mm_request_send(sock, MONITOR_ANS_KEYALLOWED, m);

	if (type == MM_RSAHOSTKEY)
		monitor_permit(mon_dispatch, MONITOR_REQ_RSACHALLENGE, allowed);

	return (0);
}
