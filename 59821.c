static long rxrpc_read(const struct key *key,
		       char __user *buffer, size_t buflen)
{
	const struct rxrpc_key_token *token;
	const struct krb5_principal *princ;
	size_t size;
	__be32 __user *xdr, *oldxdr;
	u32 cnlen, toksize, ntoks, tok, zero;
	u16 toksizes[AFSTOKEN_MAX];
	int loop;

	_enter("");

	/* we don't know what form we should return non-AFS keys in */
	if (memcmp(key->description, "afs@", 4) != 0)
		return -EOPNOTSUPP;
	cnlen = strlen(key->description + 4);

#define RND(X) (((X) + 3) & ~3)

	/* AFS keys we return in XDR form, so we need to work out the size of
	 * the XDR */
	size = 2 * 4;	/* flags, cellname len */
	size += RND(cnlen);	/* cellname */
	size += 1 * 4;	/* token count */

	ntoks = 0;
	for (token = key->payload.data; token; token = token->next) {
		toksize = 4;	/* sec index */

		switch (token->security_index) {
		case RXRPC_SECURITY_RXKAD:
			toksize += 8 * 4;	/* viceid, kvno, key*2, begin,
						 * end, primary, tktlen */
			toksize += RND(token->kad->ticket_len);
			break;

		case RXRPC_SECURITY_RXK5:
			princ = &token->k5->client;
			toksize += 4 + princ->n_name_parts * 4;
			for (loop = 0; loop < princ->n_name_parts; loop++)
				toksize += RND(strlen(princ->name_parts[loop]));
			toksize += 4 + RND(strlen(princ->realm));

			princ = &token->k5->server;
			toksize += 4 + princ->n_name_parts * 4;
			for (loop = 0; loop < princ->n_name_parts; loop++)
				toksize += RND(strlen(princ->name_parts[loop]));
			toksize += 4 + RND(strlen(princ->realm));

			toksize += 8 + RND(token->k5->session.data_len);

			toksize += 4 * 8 + 2 * 4;

			toksize += 4 + token->k5->n_addresses * 8;
			for (loop = 0; loop < token->k5->n_addresses; loop++)
				toksize += RND(token->k5->addresses[loop].data_len);

			toksize += 4 + RND(token->k5->ticket_len);
			toksize += 4 + RND(token->k5->ticket2_len);

			toksize += 4 + token->k5->n_authdata * 8;
			for (loop = 0; loop < token->k5->n_authdata; loop++)
				toksize += RND(token->k5->authdata[loop].data_len);
			break;

		default: /* we have a ticket we can't encode */
			BUG();
			continue;
		}

		_debug("token[%u]: toksize=%u", ntoks, toksize);
		ASSERTCMP(toksize, <=, AFSTOKEN_LENGTH_MAX);

		toksizes[ntoks++] = toksize;
		size += toksize + 4; /* each token has a length word */
	}

#undef RND

	if (!buffer || buflen < size)
		return size;

	xdr = (__be32 __user *) buffer;
	zero = 0;
#define ENCODE(x)				\
	do {					\
		__be32 y = htonl(x);		\
		if (put_user(y, xdr++) < 0)	\
			goto fault;		\
	} while(0)
#define ENCODE_DATA(l, s)						\
	do {								\
		u32 _l = (l);						\
		ENCODE(l);						\
		if (copy_to_user(xdr, (s), _l) != 0)			\
			goto fault;					\
		if (_l & 3 &&						\
		    copy_to_user((u8 *)xdr + _l, &zero, 4 - (_l & 3)) != 0) \
			goto fault;					\
		xdr += (_l + 3) >> 2;					\
	} while(0)
#define ENCODE64(x)					\
	do {						\
		__be64 y = cpu_to_be64(x);		\
		if (copy_to_user(xdr, &y, 8) != 0)	\
			goto fault;			\
		xdr += 8 >> 2;				\
	} while(0)
#define ENCODE_STR(s)				\
	do {					\
		const char *_s = (s);		\
		ENCODE_DATA(strlen(_s), _s);	\
	} while(0)

	ENCODE(0);					/* flags */
	ENCODE_DATA(cnlen, key->description + 4);	/* cellname */
	ENCODE(ntoks);

	tok = 0;
	for (token = key->payload.data; token; token = token->next) {
		toksize = toksizes[tok++];
		ENCODE(toksize);
		oldxdr = xdr;
		ENCODE(token->security_index);

		switch (token->security_index) {
		case RXRPC_SECURITY_RXKAD:
			ENCODE(token->kad->vice_id);
			ENCODE(token->kad->kvno);
			ENCODE_DATA(8, token->kad->session_key);
			ENCODE(token->kad->start);
			ENCODE(token->kad->expiry);
			ENCODE(token->kad->primary_flag);
			ENCODE_DATA(token->kad->ticket_len, token->kad->ticket);
			break;

		case RXRPC_SECURITY_RXK5:
			princ = &token->k5->client;
			ENCODE(princ->n_name_parts);
			for (loop = 0; loop < princ->n_name_parts; loop++)
				ENCODE_STR(princ->name_parts[loop]);
			ENCODE_STR(princ->realm);

			princ = &token->k5->server;
			ENCODE(princ->n_name_parts);
			for (loop = 0; loop < princ->n_name_parts; loop++)
				ENCODE_STR(princ->name_parts[loop]);
			ENCODE_STR(princ->realm);

			ENCODE(token->k5->session.tag);
			ENCODE_DATA(token->k5->session.data_len,
				    token->k5->session.data);

			ENCODE64(token->k5->authtime);
			ENCODE64(token->k5->starttime);
			ENCODE64(token->k5->endtime);
			ENCODE64(token->k5->renew_till);
			ENCODE(token->k5->is_skey);
			ENCODE(token->k5->flags);

			ENCODE(token->k5->n_addresses);
			for (loop = 0; loop < token->k5->n_addresses; loop++) {
				ENCODE(token->k5->addresses[loop].tag);
				ENCODE_DATA(token->k5->addresses[loop].data_len,
					    token->k5->addresses[loop].data);
			}

			ENCODE_DATA(token->k5->ticket_len, token->k5->ticket);
			ENCODE_DATA(token->k5->ticket2_len, token->k5->ticket2);

			ENCODE(token->k5->n_authdata);
			for (loop = 0; loop < token->k5->n_authdata; loop++) {
				ENCODE(token->k5->authdata[loop].tag);
				ENCODE_DATA(token->k5->authdata[loop].data_len,
					    token->k5->authdata[loop].data);
			}
			break;

		default:
			BUG();
			break;
		}

		ASSERTCMP((unsigned long)xdr - (unsigned long)oldxdr, ==,
			  toksize);
	}

#undef ENCODE_STR
#undef ENCODE_DATA
#undef ENCODE64
#undef ENCODE

	ASSERTCMP(tok, ==, ntoks);
	ASSERTCMP((char __user *) xdr - buffer, ==, size);
	_leave(" = %zu", size);
	return size;

fault:
	_leave(" = -EFAULT");
	return -EFAULT;
}
