int ssl3_get_new_session_ticket(SSL *s)
	{
	int ok,al,ret=0, ticklen;
	long n;
	const unsigned char *p;
	unsigned char *d;

	n=s->method->ssl_get_message(s,
		SSL3_ST_CR_SESSION_TICKET_A,
		SSL3_ST_CR_SESSION_TICKET_B,
		SSL3_MT_NEWSESSION_TICKET,
		16384,
		&ok);

	if (!ok)
		return((int)n);

	if (n < 6)
		{
		/* need at least ticket_lifetime_hint + ticket length */
		al = SSL_AD_DECODE_ERROR;
		SSLerr(SSL_F_SSL3_GET_NEW_SESSION_TICKET,SSL_R_LENGTH_MISMATCH);
		goto f_err;
		}

	p=d=(unsigned char *)s->init_msg;
	n2l(p, s->session->tlsext_tick_lifetime_hint);
	n2s(p, ticklen);
	/* ticket_lifetime_hint + ticket_length + ticket */
	if (ticklen + 6 != n)
		{
		al = SSL_AD_DECODE_ERROR;
		SSLerr(SSL_F_SSL3_GET_NEW_SESSION_TICKET,SSL_R_LENGTH_MISMATCH);
		goto f_err;
		}
	if (s->session->tlsext_tick)
		{
		OPENSSL_free(s->session->tlsext_tick);
		s->session->tlsext_ticklen = 0;
		}
	s->session->tlsext_tick = OPENSSL_malloc(ticklen);
	if (!s->session->tlsext_tick)
		{
		SSLerr(SSL_F_SSL3_GET_NEW_SESSION_TICKET,ERR_R_MALLOC_FAILURE);
		goto err;
		}
	memcpy(s->session->tlsext_tick, p, ticklen);
	s->session->tlsext_ticklen = ticklen;
	/* There are two ways to detect a resumed ticket session.
	 * One is to set an appropriate session ID and then the server
	 * must return a match in ServerHello. This allows the normal
	 * client session ID matching to work and we know much 
	 * earlier that the ticket has been accepted.
	 * 
	 * The other way is to set zero length session ID when the
	 * ticket is presented and rely on the handshake to determine
	 * session resumption.
	 *
	 * We choose the former approach because this fits in with
	 * assumptions elsewhere in OpenSSL. The session ID is set
	 * to the SHA256 (or SHA1 is SHA256 is disabled) hash of the
	 * ticket.
	 */ 
	EVP_Digest(p, ticklen,
			s->session->session_id, &s->session->session_id_length,
#ifndef OPENSSL_NO_SHA256
							EVP_sha256(), NULL);
#else
							EVP_sha1(), NULL);
#endif
	ret=1;
	return(ret);
f_err:
	ssl3_send_alert(s,SSL3_AL_FATAL,al);
err:
	return(-1);
	}
