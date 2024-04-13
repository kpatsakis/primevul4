static int ssl3_check_client_certificate(SSL *s)
	{
	unsigned long alg_k;
	if (!s->cert || !s->cert->key->x509 || !s->cert->key->privatekey)
		return 0;
	/* If no suitable signature algorithm can't use certificate */
	if (SSL_USE_SIGALGS(s) && !s->cert->key->digest)
		return 0;
	/* If strict mode check suitability of chain before using it.
	 * This also adjusts suite B digest if necessary.
	 */
	if (s->cert->cert_flags & SSL_CERT_FLAGS_CHECK_TLS_STRICT &&
		!tls1_check_chain(s, NULL, NULL, NULL, -2))
		return 0;
	alg_k=s->s3->tmp.new_cipher->algorithm_mkey;
	/* See if we can use client certificate for fixed DH */
	if (alg_k & (SSL_kDHr|SSL_kDHd))
		{
		SESS_CERT *scert = s->session->sess_cert;
		int i = scert->peer_cert_type;
		EVP_PKEY *clkey = NULL, *spkey = NULL;
		clkey = s->cert->key->privatekey;
		/* If client key not DH assume it can be used */
		if (EVP_PKEY_id(clkey) != EVP_PKEY_DH)
			return 1;
		if (i >= 0)
			spkey = X509_get_pubkey(scert->peer_pkeys[i].x509);
		if (spkey)
			{
			/* Compare server and client parameters */
			i = EVP_PKEY_cmp_parameters(clkey, spkey);
			EVP_PKEY_free(spkey);
			if (i != 1)
				return 0;
			}
		s->s3->flags |= TLS1_FLAGS_SKIP_CERT_VERIFY;
		}
	return 1;
	}
