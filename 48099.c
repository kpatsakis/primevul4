int ssl3_send_server_certificate(SSL *s)
	{
	CERT_PKEY *cpk;

	if (s->state == SSL3_ST_SW_CERT_A)
		{
		cpk=ssl_get_server_send_pkey(s);
		if (cpk == NULL)
			{
			/* VRS: allow null cert if auth == KRB5 */
			if ((s->s3->tmp.new_cipher->algorithm_auth != SSL_aKRB5) ||
			    (s->s3->tmp.new_cipher->algorithm_mkey & SSL_kKRB5))
				{
				SSLerr(SSL_F_SSL3_SEND_SERVER_CERTIFICATE,ERR_R_INTERNAL_ERROR);
				return(0);
				}
			}

		if (!ssl3_output_cert_chain(s,cpk))
			{
			SSLerr(SSL_F_SSL3_SEND_SERVER_CERTIFICATE,ERR_R_INTERNAL_ERROR);
			return(0);
			}
		s->state=SSL3_ST_SW_CERT_B;
		}

	/* SSL3_ST_SW_CERT_B */
	return ssl_do_write(s);
	}
