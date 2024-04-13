int dtls1_new(SSL *s)
	{
	DTLS1_STATE *d1;

	if (!ssl3_new(s)) return(0);
	if ((d1=OPENSSL_malloc(sizeof *d1)) == NULL) return (0);
	memset(d1,0, sizeof *d1);

	/* d1->handshake_epoch=0; */

	d1->unprocessed_rcds.q=pqueue_new();
	d1->processed_rcds.q=pqueue_new();
	d1->buffered_messages = pqueue_new();
	d1->sent_messages=pqueue_new();
	d1->buffered_app_data.q=pqueue_new();

	if ( s->server)
		{
		d1->cookie_len = sizeof(s->d1->cookie);
		}

	if( ! d1->unprocessed_rcds.q || ! d1->processed_rcds.q 
        || ! d1->buffered_messages || ! d1->sent_messages || ! d1->buffered_app_data.q)
		{
        if ( d1->unprocessed_rcds.q) pqueue_free(d1->unprocessed_rcds.q);
        if ( d1->processed_rcds.q) pqueue_free(d1->processed_rcds.q);
        if ( d1->buffered_messages) pqueue_free(d1->buffered_messages);
		if ( d1->sent_messages) pqueue_free(d1->sent_messages);
		if ( d1->buffered_app_data.q) pqueue_free(d1->buffered_app_data.q);
		OPENSSL_free(d1);
		return (0);
		}

	s->d1=d1;
	s->method->ssl_clear(s);
	return(1);
	}
