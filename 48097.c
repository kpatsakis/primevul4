int ssl3_send_hello_request(SSL *s)
	{

	if (s->state == SSL3_ST_SW_HELLO_REQ_A)
		{
		ssl_set_handshake_header(s, SSL3_MT_HELLO_REQUEST, 0);
		s->state=SSL3_ST_SW_HELLO_REQ_B;
		}

	/* SSL3_ST_SW_HELLO_REQ_B */
	return ssl_do_write(s);
	}
