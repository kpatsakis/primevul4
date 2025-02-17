int ssl3_get_client_hello(SSL *s)
	{
	int i,j,ok,al=SSL_AD_INTERNAL_ERROR,ret= -1;
	unsigned int cookie_len;
	long n;
	unsigned long id;
	unsigned char *p,*d;
	SSL_CIPHER *c;
#ifndef OPENSSL_NO_COMP
	unsigned char *q;
	SSL_COMP *comp=NULL;
#endif
	STACK_OF(SSL_CIPHER) *ciphers=NULL;

	if (s->state == SSL3_ST_SR_CLNT_HELLO_C && !s->first_packet)
		goto retry_cert;

	/* We do this so that we will respond with our native type.
	 * If we are TLSv1 and we get SSLv3, we will respond with TLSv1,
	 * This down switching should be handled by a different method.
	 * If we are SSLv3, we will respond with SSLv3, even if prompted with
	 * TLSv1.
	 */
	if (s->state == SSL3_ST_SR_CLNT_HELLO_A
		)
		{
		s->state=SSL3_ST_SR_CLNT_HELLO_B;
		}
	s->first_packet=1;
	n=s->method->ssl_get_message(s,
		SSL3_ST_SR_CLNT_HELLO_B,
		SSL3_ST_SR_CLNT_HELLO_C,
		SSL3_MT_CLIENT_HELLO,
		SSL3_RT_MAX_PLAIN_LENGTH,
		&ok);

	if (!ok) return((int)n);
	s->first_packet=0;
	d=p=(unsigned char *)s->init_msg;

	/* use version from inside client hello, not from record header
	 * (may differ: see RFC 2246, Appendix E, second paragraph) */
	s->client_version=(((int)p[0])<<8)|(int)p[1];
	p+=2;

	if (SSL_IS_DTLS(s)  ?	(s->client_version > s->version &&
				 s->method->version != DTLS_ANY_VERSION)
			    :	(s->client_version < s->version))
		{
		SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO, SSL_R_WRONG_VERSION_NUMBER);
		if ((s->client_version>>8) == SSL3_VERSION_MAJOR &&
			!s->enc_write_ctx && !s->write_hash)
			{
			/* similar to ssl3_get_record, send alert using remote version number */
			s->version = s->client_version;
			}
		al = SSL_AD_PROTOCOL_VERSION;
		goto f_err;
		}

	/* If we require cookies and this ClientHello doesn't
	 * contain one, just return since we do not want to
	 * allocate any memory yet. So check cookie length...
	 */
	if (SSL_get_options(s) & SSL_OP_COOKIE_EXCHANGE)
		{
		unsigned int session_length, cookie_length;
		
		session_length = *(p + SSL3_RANDOM_SIZE);
		cookie_length = *(p + SSL3_RANDOM_SIZE + session_length + 1);

		if (cookie_length == 0)
			return 1;
		}

	/* load the client random */
	memcpy(s->s3->client_random,p,SSL3_RANDOM_SIZE);
	p+=SSL3_RANDOM_SIZE;

	/* get the session-id */
	j= *(p++);

	s->hit=0;
	/* Versions before 0.9.7 always allow clients to resume sessions in renegotiation.
	 * 0.9.7 and later allow this by default, but optionally ignore resumption requests
	 * with flag SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION (it's a new flag rather
	 * than a change to default behavior so that applications relying on this for security
	 * won't even compile against older library versions).
	 *
	 * 1.0.1 and later also have a function SSL_renegotiate_abbreviated() to request
	 * renegotiation but not a new session (s->new_session remains unset): for servers,
	 * this essentially just means that the SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION
	 * setting will be ignored.
	 */
	if ((s->new_session && (s->options & SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION)))
		{
		if (!ssl_get_new_session(s,1))
			goto err;
		}
	else
		{
		i=ssl_get_prev_session(s, p, j, d + n);
		/*
		 * Only resume if the session's version matches the negotiated
		 * version.
		 * RFC 5246 does not provide much useful advice on resumption
		 * with a different protocol version. It doesn't forbid it but
		 * the sanity of such behaviour would be questionable.
		 * In practice, clients do not accept a version mismatch and
		 * will abort the handshake with an error.
		 */
		if (i == 1 && s->version == s->session->ssl_version)
			{ /* previous session */
			s->hit=1;
			}
		else if (i == -1)
			goto err;
		else /* i == 0 */
			{
			if (!ssl_get_new_session(s,1))
				goto err;
			}
		}

	p+=j;

	if (SSL_IS_DTLS(s))
		{
		/* cookie stuff */
		cookie_len = *(p++);

		/* 
		 * The ClientHello may contain a cookie even if the
		 * HelloVerify message has not been sent--make sure that it
		 * does not cause an overflow.
		 */
		if ( cookie_len > sizeof(s->d1->rcvd_cookie))
			{
			/* too much data */
			al = SSL_AD_DECODE_ERROR;
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO, SSL_R_COOKIE_MISMATCH);
			goto f_err;
			}

		/* verify the cookie if appropriate option is set. */
		if ((SSL_get_options(s) & SSL_OP_COOKIE_EXCHANGE) &&
			cookie_len > 0)
			{
			memcpy(s->d1->rcvd_cookie, p, cookie_len);

			if ( s->ctx->app_verify_cookie_cb != NULL)
				{
				if ( s->ctx->app_verify_cookie_cb(s, s->d1->rcvd_cookie,
					cookie_len) == 0)
					{
					al=SSL_AD_HANDSHAKE_FAILURE;
					SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO, 
						SSL_R_COOKIE_MISMATCH);
					goto f_err;
					}
				/* else cookie verification succeeded */
				}
			else if ( memcmp(s->d1->rcvd_cookie, s->d1->cookie, 
						  s->d1->cookie_len) != 0) /* default verification */
				{
					al=SSL_AD_HANDSHAKE_FAILURE;
					SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO, 
						SSL_R_COOKIE_MISMATCH);
					goto f_err;
				}
			/* Set to -2 so if successful we return 2 */
			ret = -2;
			}

		p += cookie_len;
		if (s->method->version == DTLS_ANY_VERSION)
			{
			/* Select version to use */
			if (s->client_version <= DTLS1_2_VERSION &&
				!(s->options & SSL_OP_NO_DTLSv1_2))
				{
				s->version = DTLS1_2_VERSION;
				s->method = DTLSv1_2_server_method();
				}
			else if (tls1_suiteb(s))
				{
				SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO, SSL_R_ONLY_DTLS_1_2_ALLOWED_IN_SUITEB_MODE);
				s->version = s->client_version;
				al = SSL_AD_PROTOCOL_VERSION;
				goto f_err;
				}
			else if (s->client_version <= DTLS1_VERSION &&
				!(s->options & SSL_OP_NO_DTLSv1))
				{
				s->version = DTLS1_VERSION;
				s->method = DTLSv1_server_method();
				}
			else
				{
				SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO, SSL_R_WRONG_VERSION_NUMBER);
				s->version = s->client_version;
				al = SSL_AD_PROTOCOL_VERSION;
				goto f_err;
				}
			s->session->ssl_version = s->version;
			}
		}

	n2s(p,i);
	if ((i == 0) && (j != 0))
		{
		/* we need a cipher if we are not resuming a session */
		al=SSL_AD_ILLEGAL_PARAMETER;
		SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_NO_CIPHERS_SPECIFIED);
		goto f_err;
		}
	if ((p+i) >= (d+n))
		{
		/* not enough data */
		al=SSL_AD_DECODE_ERROR;
		SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_LENGTH_MISMATCH);
		goto f_err;
		}
	if ((i > 0) && (ssl_bytes_to_cipher_list(s,p,i,&(ciphers))
		== NULL))
		{
		goto err;
		}
	p+=i;

	/* If it is a hit, check that the cipher is in the list */
	if ((s->hit) && (i > 0))
		{
		j=0;
		id=s->session->cipher->id;

#ifdef CIPHER_DEBUG
		fprintf(stderr,"client sent %d ciphers\n",sk_SSL_CIPHER_num(ciphers));
#endif
		for (i=0; i<sk_SSL_CIPHER_num(ciphers); i++)
			{
			c=sk_SSL_CIPHER_value(ciphers,i);
#ifdef CIPHER_DEBUG
			fprintf(stderr,"client [%2d of %2d]:%s\n",
				i,sk_SSL_CIPHER_num(ciphers),
				SSL_CIPHER_get_name(c));
#endif
			if (c->id == id)
				{
				j=1;
				break;
				}
			}
/* Disabled because it can be used in a ciphersuite downgrade
 * attack: CVE-2010-4180.
 */
#if 0
		if (j == 0 && (s->options & SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG) && (sk_SSL_CIPHER_num(ciphers) == 1))
			{
			/* Special case as client bug workaround: the previously used cipher may
			 * not be in the current list, the client instead might be trying to
			 * continue using a cipher that before wasn't chosen due to server
			 * preferences.  We'll have to reject the connection if the cipher is not
			 * enabled, though. */
			c = sk_SSL_CIPHER_value(ciphers, 0);
			if (sk_SSL_CIPHER_find(SSL_get_ciphers(s), c) >= 0)
				{
				s->session->cipher = c;
				j = 1;
				}
			}
#endif
		if (j == 0)
			{
			/* we need to have the cipher in the cipher
			 * list if we are asked to reuse it */
			al=SSL_AD_ILLEGAL_PARAMETER;
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_REQUIRED_CIPHER_MISSING);
			goto f_err;
			}
		}

	/* compression */
	i= *(p++);
	if ((p+i) > (d+n))
		{
		/* not enough data */
		al=SSL_AD_DECODE_ERROR;
		SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_LENGTH_MISMATCH);
		goto f_err;
		}
#ifndef OPENSSL_NO_COMP
	q=p;
#endif
	for (j=0; j<i; j++)
		{
		if (p[j] == 0) break;
		}

	p+=i;
	if (j >= i)
		{
		/* no compress */
		al=SSL_AD_DECODE_ERROR;
		SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_NO_COMPRESSION_SPECIFIED);
		goto f_err;
		}

#ifndef OPENSSL_NO_TLSEXT
	/* TLS extensions*/
	if (s->version >= SSL3_VERSION)
		{
		if (!ssl_parse_clienthello_tlsext(s,&p,d,n))
			{
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_PARSE_TLSEXT);
			goto err;
			}
		}

	/* Check if we want to use external pre-shared secret for this
	 * handshake for not reused session only. We need to generate
	 * server_random before calling tls_session_secret_cb in order to allow
	 * SessionTicket processing to use it in key derivation. */
	{
		unsigned char *pos;
		pos=s->s3->server_random;
		if (ssl_fill_hello_random(s, 1, pos, SSL3_RANDOM_SIZE) <= 0)
			{
			goto f_err;
			}
	}

	if (!s->hit && s->version >= TLS1_VERSION && s->tls_session_secret_cb)
		{
		SSL_CIPHER *pref_cipher=NULL;

		s->session->master_key_length=sizeof(s->session->master_key);
		if(s->tls_session_secret_cb(s, s->session->master_key, &s->session->master_key_length,
			ciphers, &pref_cipher, s->tls_session_secret_cb_arg))
			{
			s->hit=1;
			s->session->ciphers=ciphers;
			s->session->verify_result=X509_V_OK;

			ciphers=NULL;

			/* check if some cipher was preferred by call back */
			pref_cipher=pref_cipher ? pref_cipher : ssl3_choose_cipher(s, s->session->ciphers, SSL_get_ciphers(s));
			if (pref_cipher == NULL)
				{
				al=SSL_AD_HANDSHAKE_FAILURE;
				SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_NO_SHARED_CIPHER);
				goto f_err;
				}

			s->session->cipher=pref_cipher;

			if (s->cipher_list)
				sk_SSL_CIPHER_free(s->cipher_list);

			if (s->cipher_list_by_id)
				sk_SSL_CIPHER_free(s->cipher_list_by_id);

			s->cipher_list = sk_SSL_CIPHER_dup(s->session->ciphers);
			s->cipher_list_by_id = sk_SSL_CIPHER_dup(s->session->ciphers);
			}
		}
#endif

	/* Worst case, we will use the NULL compression, but if we have other
	 * options, we will now look for them.  We have i-1 compression
	 * algorithms from the client, starting at q. */
	s->s3->tmp.new_compression=NULL;
#ifndef OPENSSL_NO_COMP
	/* This only happens if we have a cache hit */
	if (s->session->compress_meth != 0)
		{
		int m, comp_id = s->session->compress_meth;
		/* Perform sanity checks on resumed compression algorithm */
		/* Can't disable compression */
		if (!ssl_allow_compression(s))
			{
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_INCONSISTENT_COMPRESSION);
			goto f_err;
			}
		/* Look for resumed compression method */
		for (m = 0; m < sk_SSL_COMP_num(s->ctx->comp_methods); m++)
			{
			comp=sk_SSL_COMP_value(s->ctx->comp_methods,m);
			if (comp_id == comp->id)
				{
				s->s3->tmp.new_compression=comp;
				break;
				}
			}
		if (s->s3->tmp.new_compression == NULL)
			{
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_INVALID_COMPRESSION_ALGORITHM);
			goto f_err;
			}
		/* Look for resumed method in compression list */
		for (m = 0; m < i; m++)
			{
			if (q[m] == comp_id)
				break;
			}
		if (m >= i)
			{
			al=SSL_AD_ILLEGAL_PARAMETER;
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_REQUIRED_COMPRESSSION_ALGORITHM_MISSING);
			goto f_err;
			}
		}
	else if (s->hit)
		comp = NULL;
	else if (ssl_allow_compression(s) && s->ctx->comp_methods)
		{ /* See if we have a match */
		int m,nn,o,v,done=0;

		nn=sk_SSL_COMP_num(s->ctx->comp_methods);
		for (m=0; m<nn; m++)
			{
			comp=sk_SSL_COMP_value(s->ctx->comp_methods,m);
			v=comp->id;
			for (o=0; o<i; o++)
				{
				if (v == q[o])
					{
					done=1;
					break;
					}
				}
			if (done) break;
			}
		if (done)
			s->s3->tmp.new_compression=comp;
		else
			comp=NULL;
		}
#else
	/* If compression is disabled we'd better not try to resume a session
	 * using compression.
	 */
	if (s->session->compress_meth != 0)
		{
		SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_INCONSISTENT_COMPRESSION);
		goto f_err;
		}
#endif

	/* Given s->session->ciphers and SSL_get_ciphers, we must
	 * pick a cipher */

	if (!s->hit)
		{
#ifdef OPENSSL_NO_COMP
		s->session->compress_meth=0;
#else
		s->session->compress_meth=(comp == NULL)?0:comp->id;
#endif
		if (s->session->ciphers != NULL)
			sk_SSL_CIPHER_free(s->session->ciphers);
		s->session->ciphers=ciphers;
		if (ciphers == NULL)
			{
			al=SSL_AD_ILLEGAL_PARAMETER;
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_NO_CIPHERS_PASSED);
			goto f_err;
			}
		ciphers=NULL;
		if (!tls1_set_server_sigalgs(s))
			{
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO, SSL_R_CLIENTHELLO_TLSEXT);
			goto err;
			}
		/* Let cert callback update server certificates if required */
		retry_cert:		
		if (s->cert->cert_cb)
			{
			int rv = s->cert->cert_cb(s, s->cert->cert_cb_arg);
			if (rv == 0)
				{
				al=SSL_AD_INTERNAL_ERROR;
				SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_CERT_CB_ERROR);
				goto f_err;
				}
			if (rv < 0)
				{
				s->rwstate=SSL_X509_LOOKUP;
				return -1;
				}
			s->rwstate = SSL_NOTHING;
			}
		c=ssl3_choose_cipher(s,s->session->ciphers,
				     SSL_get_ciphers(s));

		if (c == NULL)
			{
			al=SSL_AD_HANDSHAKE_FAILURE;
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO,SSL_R_NO_SHARED_CIPHER);
			goto f_err;
			}
		s->s3->tmp.new_cipher=c;
		/* check whether we should disable session resumption */
		if (s->not_resumable_session_cb != NULL)
			s->session->not_resumable=s->not_resumable_session_cb(s,
				((c->algorithm_mkey & (SSL_kDHE | SSL_kECDHE)) != 0));
		if (s->session->not_resumable)
			/* do not send a session ticket */
			s->tlsext_ticket_expected = 0;
		}
	else
		{
		/* Session-id reuse */
#ifdef REUSE_CIPHER_BUG
		STACK_OF(SSL_CIPHER) *sk;
		SSL_CIPHER *nc=NULL;
		SSL_CIPHER *ec=NULL;

		if (s->options & SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG)
			{
			sk=s->session->ciphers;
			for (i=0; i<sk_SSL_CIPHER_num(sk); i++)
				{
				c=sk_SSL_CIPHER_value(sk,i);
				if (c->algorithm_enc & SSL_eNULL)
					nc=c;
				if (SSL_C_IS_EXPORT(c))
					ec=c;
				}
			if (nc != NULL)
				s->s3->tmp.new_cipher=nc;
			else if (ec != NULL)
				s->s3->tmp.new_cipher=ec;
			else
				s->s3->tmp.new_cipher=s->session->cipher;
			}
		else
#endif
		s->s3->tmp.new_cipher=s->session->cipher;
		}

	if (!SSL_USE_SIGALGS(s) || !(s->verify_mode & SSL_VERIFY_PEER))
		{
		if (!ssl3_digest_cached_records(s))
			goto f_err;
		}
	
	/*-
	 * we now have the following setup. 
	 * client_random
	 * cipher_list 		- our prefered list of ciphers
	 * ciphers 		- the clients prefered list of ciphers
	 * compression		- basically ignored right now
	 * ssl version is set	- sslv3
	 * s->session		- The ssl session has been setup.
	 * s->hit		- session reuse flag
	 * s->s3->tmp.new_cipher- the new cipher to use.
	 */

	/* Handles TLS extensions that we couldn't check earlier */
	if (s->version >= SSL3_VERSION)
		{
		if (ssl_check_clienthello_tlsext_late(s) <= 0)
			{
			SSLerr(SSL_F_SSL3_GET_CLIENT_HELLO, SSL_R_CLIENTHELLO_TLSEXT);
			goto err;
			}
		}

	if (ret < 0) ret=-ret;
	if (0)
		{
f_err:
		ssl3_send_alert(s,SSL3_AL_FATAL,al);
		}
err:
	if (ciphers != NULL) sk_SSL_CIPHER_free(ciphers);
	return ret < 0 ? -1 : ret;
	}
