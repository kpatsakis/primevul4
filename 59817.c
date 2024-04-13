static int rxrpc_preparse(struct key_preparsed_payload *prep)
{
	const struct rxrpc_key_data_v1 *v1;
	struct rxrpc_key_token *token, **pp;
	size_t plen;
	u32 kver;
	int ret;

	_enter("%zu", prep->datalen);

	/* handle a no-security key */
	if (!prep->data && prep->datalen == 0)
		return 0;

	/* determine if the XDR payload format is being used */
	if (prep->datalen > 7 * 4) {
		ret = rxrpc_preparse_xdr(prep);
		if (ret != -EPROTO)
			return ret;
	}

	/* get the key interface version number */
	ret = -EINVAL;
	if (prep->datalen <= 4 || !prep->data)
		goto error;
	memcpy(&kver, prep->data, sizeof(kver));
	prep->data += sizeof(kver);
	prep->datalen -= sizeof(kver);

	_debug("KEY I/F VERSION: %u", kver);

	ret = -EKEYREJECTED;
	if (kver != 1)
		goto error;

	/* deal with a version 1 key */
	ret = -EINVAL;
	if (prep->datalen < sizeof(*v1))
		goto error;

	v1 = prep->data;
	if (prep->datalen != sizeof(*v1) + v1->ticket_length)
		goto error;

	_debug("SCIX: %u", v1->security_index);
	_debug("TLEN: %u", v1->ticket_length);
	_debug("EXPY: %x", v1->expiry);
	_debug("KVNO: %u", v1->kvno);
	_debug("SKEY: %02x%02x%02x%02x%02x%02x%02x%02x",
	       v1->session_key[0], v1->session_key[1],
	       v1->session_key[2], v1->session_key[3],
	       v1->session_key[4], v1->session_key[5],
	       v1->session_key[6], v1->session_key[7]);
	if (v1->ticket_length >= 8)
		_debug("TCKT: %02x%02x%02x%02x%02x%02x%02x%02x",
		       v1->ticket[0], v1->ticket[1],
		       v1->ticket[2], v1->ticket[3],
		       v1->ticket[4], v1->ticket[5],
		       v1->ticket[6], v1->ticket[7]);

	ret = -EPROTONOSUPPORT;
	if (v1->security_index != RXRPC_SECURITY_RXKAD)
		goto error;

	plen = sizeof(*token->kad) + v1->ticket_length;
	prep->quotalen = plen + sizeof(*token);

	ret = -ENOMEM;
	token = kzalloc(sizeof(*token), GFP_KERNEL);
	if (!token)
		goto error;
	token->kad = kzalloc(plen, GFP_KERNEL);
	if (!token->kad)
		goto error_free;

	token->security_index		= RXRPC_SECURITY_RXKAD;
	token->kad->ticket_len		= v1->ticket_length;
	token->kad->expiry		= v1->expiry;
	token->kad->kvno		= v1->kvno;
	memcpy(&token->kad->session_key, &v1->session_key, 8);
	memcpy(&token->kad->ticket, v1->ticket, v1->ticket_length);

	/* count the number of tokens attached */
	prep->type_data[0] = (void *)((unsigned long)prep->type_data[0] + 1);

	/* attach the data */
	pp = (struct rxrpc_key_token **)&prep->payload[0];
	while (*pp)
		pp = &(*pp)->next;
	*pp = token;
	if (token->kad->expiry < prep->expiry)
		prep->expiry = token->kad->expiry;
	token = NULL;
	ret = 0;

error_free:
	kfree(token);
error:
	return ret;
}
