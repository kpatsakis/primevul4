int ssl3_send_cert_status(SSL *s)
	{
	if (s->state == SSL3_ST_SW_CERT_STATUS_A)
		{
		unsigned char *p;
		/*-
		 * Grow buffer if need be: the length calculation is as
 		 * follows 1 (message type) + 3 (message length) +
 		 * 1 (ocsp response type) + 3 (ocsp response length)
 		 * + (ocsp response)
 		 */
		if (!BUF_MEM_grow(s->init_buf, 8 + s->tlsext_ocsp_resplen))
			return -1;

		p=(unsigned char *)s->init_buf->data;

		/* do the header */
		*(p++)=SSL3_MT_CERTIFICATE_STATUS;
		/* message length */
		l2n3(s->tlsext_ocsp_resplen + 4, p);
		/* status type */
		*(p++)= s->tlsext_status_type;
		/* length of OCSP response */
		l2n3(s->tlsext_ocsp_resplen, p);
		/* actual response */
		memcpy(p, s->tlsext_ocsp_resp, s->tlsext_ocsp_resplen);
		/* number of bytes to write */
		s->init_num = 8 + s->tlsext_ocsp_resplen;
		s->state=SSL3_ST_SW_CERT_STATUS_B;
		s->init_off = 0;
		}

	/* SSL3_ST_SW_CERT_STATUS_B */
	return(ssl3_do_write(s,SSL3_RT_HANDSHAKE));
	}
