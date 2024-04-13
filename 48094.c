int ssl3_get_next_proto(SSL *s)
	{
	int ok;
	int proto_len, padding_len;
	long n;
	const unsigned char *p;

	/* Clients cannot send a NextProtocol message if we didn't see the
	 * extension in their ClientHello */
	if (!s->s3->next_proto_neg_seen)
		{
		SSLerr(SSL_F_SSL3_GET_NEXT_PROTO,SSL_R_GOT_NEXT_PROTO_WITHOUT_EXTENSION);
		return -1;
		}

	n=s->method->ssl_get_message(s,
		SSL3_ST_SR_NEXT_PROTO_A,
		SSL3_ST_SR_NEXT_PROTO_B,
		SSL3_MT_NEXT_PROTO,
		514,  /* See the payload format below */
		&ok);

	if (!ok)
		return((int)n);

	/* s->state doesn't reflect whether ChangeCipherSpec has been received
	 * in this handshake, but s->s3->change_cipher_spec does (will be reset
	 * by ssl3_get_finished). */
	if (!s->s3->change_cipher_spec)
		{
		SSLerr(SSL_F_SSL3_GET_NEXT_PROTO,SSL_R_GOT_NEXT_PROTO_BEFORE_A_CCS);
		return -1;
		}

	if (n < 2)
		return 0;  /* The body must be > 1 bytes long */

	p=(unsigned char *)s->init_msg;

	/*-
	 * The payload looks like:
	 *   uint8 proto_len;
	 *   uint8 proto[proto_len];
	 *   uint8 padding_len;
	 *   uint8 padding[padding_len];
	 */
	proto_len = p[0];
	if (proto_len + 2 > s->init_num)
		return 0;
	padding_len = p[proto_len + 1];
	if (proto_len + padding_len + 2 != s->init_num)
		return 0;

	s->next_proto_negotiated = OPENSSL_malloc(proto_len);
	if (!s->next_proto_negotiated)
		{
		SSLerr(SSL_F_SSL3_GET_NEXT_PROTO,ERR_R_MALLOC_FAILURE);
		return 0;
		}
	memcpy(s->next_proto_negotiated, p + 1, proto_len);
	s->next_proto_negotiated_len = proto_len;

	return 1;
	}
