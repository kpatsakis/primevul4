void dtls1_clear(SSL *s)
	{
    pqueue unprocessed_rcds;
    pqueue processed_rcds;
    pqueue buffered_messages;
	pqueue sent_messages;
	pqueue buffered_app_data;
	unsigned int mtu;

	if (s->d1)
		{
		unprocessed_rcds = s->d1->unprocessed_rcds.q;
		processed_rcds = s->d1->processed_rcds.q;
		buffered_messages = s->d1->buffered_messages;
		sent_messages = s->d1->sent_messages;
		buffered_app_data = s->d1->buffered_app_data.q;
		mtu = s->d1->mtu;

		dtls1_clear_queues(s);

		memset(s->d1, 0, sizeof(*(s->d1)));

		if (s->server)
			{
			s->d1->cookie_len = sizeof(s->d1->cookie);
			}

		if (SSL_get_options(s) & SSL_OP_NO_QUERY_MTU)
			{
			s->d1->mtu = mtu;
			}

		s->d1->unprocessed_rcds.q = unprocessed_rcds;
		s->d1->processed_rcds.q = processed_rcds;
		s->d1->buffered_messages = buffered_messages;
		s->d1->sent_messages = sent_messages;
		s->d1->buffered_app_data.q = buffered_app_data;
		}

	ssl3_clear(s);
	if (s->options & SSL_OP_CISCO_ANYCONNECT)
		s->version=DTLS1_BAD_VER;
	else if (s->method->version == DTLS_ANY_VERSION)
		s->version=DTLS1_2_VERSION;
	else
		s->version=s->method->version;
	}
