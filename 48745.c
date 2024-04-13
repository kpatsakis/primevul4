int dtls1_check_timeout_num(SSL *s)
	{
	s->d1->timeout.num_alerts++;

	/* Reduce MTU after 2 unsuccessful retransmissions */
	if (s->d1->timeout.num_alerts > 2)
		{
		s->d1->mtu = BIO_ctrl(SSL_get_wbio(s), BIO_CTRL_DGRAM_GET_FALLBACK_MTU, 0, NULL);		
		}

	if (s->d1->timeout.num_alerts > DTLS1_TMO_ALERT_COUNT)
		{
		/* fail the connection, enough alerts have been sent */
		SSLerr(SSL_F_DTLS1_CHECK_TIMEOUT_NUM,SSL_R_READ_TIMEOUT_EXPIRED);
		return -1;
		}

	return 0;
	}
