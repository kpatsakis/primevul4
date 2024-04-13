mm_answer_sign(int sock, Buffer *m)
{
	struct ssh *ssh = active_state; 	/* XXX */
	extern int auth_sock;			/* XXX move to state struct? */
	struct sshkey *key;
	struct sshbuf *sigbuf;
	u_char *p;
	u_char *signature;
	size_t datlen, siglen;
	int r, keyid, is_proof = 0;
	const char proof_req[] = "hostkeys-prove-00@openssh.com";

	debug3("%s", __func__);

	if ((r = sshbuf_get_u32(m, &keyid)) != 0 ||
	    (r = sshbuf_get_string(m, &p, &datlen)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	/*
	 * Supported KEX types use SHA1 (20 bytes), SHA256 (32 bytes),
	 * SHA384 (48 bytes) and SHA512 (64 bytes).
	 *
	 * Otherwise, verify the signature request is for a hostkey
	 * proof.
	 *
	 * XXX perform similar check for KEX signature requests too?
	 * it's not trivial, since what is signed is the hash, rather
	 * than the full kex structure...
	 */
	if (datlen != 20 && datlen != 32 && datlen != 48 && datlen != 64) {
		/*
		 * Construct expected hostkey proof and compare it to what
		 * the client sent us.
		 */
		if (session_id2_len == 0) /* hostkeys is never first */
			fatal("%s: bad data length: %zu", __func__, datlen);
		if ((key = get_hostkey_public_by_index(keyid, ssh)) == NULL)
			fatal("%s: no hostkey for index %d", __func__, keyid);
		if ((sigbuf = sshbuf_new()) == NULL)
			fatal("%s: sshbuf_new", __func__);
		if ((r = sshbuf_put_cstring(sigbuf, proof_req)) != 0 ||
		    (r = sshbuf_put_string(sigbuf, session_id2,
		    session_id2_len) != 0) ||
		    (r = sshkey_puts(key, sigbuf)) != 0)
			fatal("%s: couldn't prepare private key "
			    "proof buffer: %s", __func__, ssh_err(r));
		if (datlen != sshbuf_len(sigbuf) ||
		    memcmp(p, sshbuf_ptr(sigbuf), sshbuf_len(sigbuf)) != 0)
			fatal("%s: bad data length: %zu, hostkey proof len %zu",
			    __func__, datlen, sshbuf_len(sigbuf));
		sshbuf_free(sigbuf);
		is_proof = 1;
	}

	/* save session id, it will be passed on the first call */
	if (session_id2_len == 0) {
		session_id2_len = datlen;
		session_id2 = xmalloc(session_id2_len);
		memcpy(session_id2, p, session_id2_len);
	}

	if ((key = get_hostkey_by_index(keyid)) != NULL) {
		if ((r = sshkey_sign(key, &signature, &siglen, p, datlen,
		    datafellows)) != 0)
			fatal("%s: sshkey_sign failed: %s",
			    __func__, ssh_err(r));
	} else if ((key = get_hostkey_public_by_index(keyid, ssh)) != NULL &&
	    auth_sock > 0) {
		if ((r = ssh_agent_sign(auth_sock, key, &signature, &siglen,
		    p, datlen, datafellows)) != 0) {
			fatal("%s: ssh_agent_sign failed: %s",
			    __func__, ssh_err(r));
		}
	} else
		fatal("%s: no hostkey from index %d", __func__, keyid);

	debug3("%s: %s signature %p(%zu)", __func__,
	    is_proof ? "KEX" : "hostkey proof", signature, siglen);

	sshbuf_reset(m);
	if ((r = sshbuf_put_string(m, signature, siglen)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	free(p);
	free(signature);

	mm_request_send(sock, MONITOR_ANS_SIGN, m);

	/* Turn on permissions for getpwnam */
	monitor_permit(mon_dispatch, MONITOR_REQ_PWNAM, 1);

	return (0);
}
