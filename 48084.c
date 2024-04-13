int dtls1_dispatch_alert(SSL *s)
	{
	int i,j;
	void (*cb)(const SSL *ssl,int type,int val)=NULL;
	unsigned char buf[DTLS1_AL_HEADER_LENGTH];
	unsigned char *ptr = &buf[0];

	s->s3->alert_dispatch=0;

	memset(buf, 0x00, sizeof(buf));
	*ptr++ = s->s3->send_alert[0];
	*ptr++ = s->s3->send_alert[1];

#ifdef DTLS1_AD_MISSING_HANDSHAKE_MESSAGE
	if (s->s3->send_alert[1] == DTLS1_AD_MISSING_HANDSHAKE_MESSAGE)
		{	
		s2n(s->d1->handshake_read_seq, ptr);
#if 0
		if ( s->d1->r_msg_hdr.frag_off == 0)  /* waiting for a new msg */

		else
			s2n(s->d1->r_msg_hdr.seq, ptr); /* partial msg read */
#endif

#if 0
		fprintf(stderr, "s->d1->handshake_read_seq = %d, s->d1->r_msg_hdr.seq = %d\n",s->d1->handshake_read_seq,s->d1->r_msg_hdr.seq);
#endif
		l2n3(s->d1->r_msg_hdr.frag_off, ptr);
		}
#endif

	i = do_dtls1_write(s, SSL3_RT_ALERT, &buf[0], sizeof(buf), 0);
	if (i <= 0)
		{
		s->s3->alert_dispatch=1;
		/* fprintf( stderr, "not done with alert\n" ); */
		}
	else
		{
		if (s->s3->send_alert[0] == SSL3_AL_FATAL
#ifdef DTLS1_AD_MISSING_HANDSHAKE_MESSAGE
		    || s->s3->send_alert[1] == DTLS1_AD_MISSING_HANDSHAKE_MESSAGE
#endif
		    )
			(void)BIO_flush(s->wbio);

		if (s->msg_callback)
			s->msg_callback(1, s->version, SSL3_RT_ALERT, s->s3->send_alert, 
				2, s, s->msg_callback_arg);

		if (s->info_callback != NULL)
			cb=s->info_callback;
		else if (s->ctx->info_callback != NULL)
			cb=s->ctx->info_callback;

		if (cb != NULL)
			{
			j=(s->s3->send_alert[0]<<8)|s->s3->send_alert[1];
			cb(s,SSL_CB_WRITE_ALERT,j);
			}
		}
	return(i);
	}
