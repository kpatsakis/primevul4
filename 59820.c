static int rxrpc_preparse_xdr_rxkad(struct key_preparsed_payload *prep,
				    size_t datalen,
				    const __be32 *xdr, unsigned int toklen)
{
	struct rxrpc_key_token *token, **pptoken;
	size_t plen;
	u32 tktlen;

	_enter(",{%x,%x,%x,%x},%u",
	       ntohl(xdr[0]), ntohl(xdr[1]), ntohl(xdr[2]), ntohl(xdr[3]),
	       toklen);

	if (toklen <= 8 * 4)
		return -EKEYREJECTED;
	tktlen = ntohl(xdr[7]);
	_debug("tktlen: %x", tktlen);
	if (tktlen > AFSTOKEN_RK_TIX_MAX)
		return -EKEYREJECTED;
	if (toklen < 8 * 4 + tktlen)
		return -EKEYREJECTED;

	plen = sizeof(*token) + sizeof(*token->kad) + tktlen;
	prep->quotalen = datalen + plen;

	plen -= sizeof(*token);
	token = kzalloc(sizeof(*token), GFP_KERNEL);
	if (!token)
		return -ENOMEM;

	token->kad = kzalloc(plen, GFP_KERNEL);
	if (!token->kad) {
		kfree(token);
		return -ENOMEM;
	}

	token->security_index	= RXRPC_SECURITY_RXKAD;
	token->kad->ticket_len	= tktlen;
	token->kad->vice_id	= ntohl(xdr[0]);
	token->kad->kvno	= ntohl(xdr[1]);
	token->kad->start	= ntohl(xdr[4]);
	token->kad->expiry	= ntohl(xdr[5]);
	token->kad->primary_flag = ntohl(xdr[6]);
	memcpy(&token->kad->session_key, &xdr[2], 8);
	memcpy(&token->kad->ticket, &xdr[8], tktlen);

	_debug("SCIX: %u", token->security_index);
	_debug("TLEN: %u", token->kad->ticket_len);
	_debug("EXPY: %x", token->kad->expiry);
	_debug("KVNO: %u", token->kad->kvno);
	_debug("PRIM: %u", token->kad->primary_flag);
	_debug("SKEY: %02x%02x%02x%02x%02x%02x%02x%02x",
	       token->kad->session_key[0], token->kad->session_key[1],
	       token->kad->session_key[2], token->kad->session_key[3],
	       token->kad->session_key[4], token->kad->session_key[5],
	       token->kad->session_key[6], token->kad->session_key[7]);
	if (token->kad->ticket_len >= 8)
		_debug("TCKT: %02x%02x%02x%02x%02x%02x%02x%02x",
		       token->kad->ticket[0], token->kad->ticket[1],
		       token->kad->ticket[2], token->kad->ticket[3],
		       token->kad->ticket[4], token->kad->ticket[5],
		       token->kad->ticket[6], token->kad->ticket[7]);

	/* count the number of tokens attached */
	prep->type_data[0] = (void *)((unsigned long)prep->type_data[0] + 1);

	/* attach the data */
	for (pptoken = (struct rxrpc_key_token **)&prep->payload[0];
	     *pptoken;
	     pptoken = &(*pptoken)->next)
		continue;
	*pptoken = token;
	if (token->kad->expiry < prep->expiry)
		prep->expiry = token->kad->expiry;

	_leave(" = 0");
	return 0;
}
