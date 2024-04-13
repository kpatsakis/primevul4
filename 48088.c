dtls1_write_app_data_bytes(SSL *s, int type, const void *buf_, int len)
	{
	int i;

#ifndef OPENSSL_NO_SCTP
		/* Check if we have to continue an interrupted handshake
		 * for reading belated app data with SCTP.
		 */
		if ((SSL_in_init(s) && !s->in_handshake) ||
		    (BIO_dgram_is_sctp(SSL_get_wbio(s)) &&
		     (s->state == DTLS1_SCTP_ST_SR_READ_SOCK || s->state == DTLS1_SCTP_ST_CR_READ_SOCK)))
#else
		if (SSL_in_init(s) && !s->in_handshake)
#endif
		{
		i=s->handshake_func(s);
		if (i < 0) return(i);
		if (i == 0)
			{
			SSLerr(SSL_F_DTLS1_WRITE_APP_DATA_BYTES,SSL_R_SSL_HANDSHAKE_FAILURE);
			return -1;
			}
		}

	if (len > SSL3_RT_MAX_PLAIN_LENGTH)
		{
			SSLerr(SSL_F_DTLS1_WRITE_APP_DATA_BYTES,SSL_R_DTLS_MESSAGE_TOO_BIG);
			return -1;
		}

	i = dtls1_write_bytes(s, type, buf_, len);
	return i;
	}
