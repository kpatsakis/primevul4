static int rxrpc_krb5_decode_tagged_data(struct krb5_tagged_data *td,
					 size_t max_data_size,
					 const __be32 **_xdr,
					 unsigned int *_toklen)
{
	const __be32 *xdr = *_xdr;
	unsigned int toklen = *_toklen, len;

	/* there must be at least one tag and one length word */
	if (toklen <= 8)
		return -EINVAL;

	_enter(",%zu,{%x,%x},%u",
	       max_data_size, ntohl(xdr[0]), ntohl(xdr[1]), toklen);

	td->tag = ntohl(*xdr++);
	len = ntohl(*xdr++);
	toklen -= 8;
	if (len > max_data_size)
		return -EINVAL;
	td->data_len = len;

	if (len > 0) {
		td->data = kmemdup(xdr, len, GFP_KERNEL);
		if (!td->data)
			return -ENOMEM;
		len = (len + 3) & ~3;
		toklen -= len;
		xdr += len >> 2;
	}

	_debug("tag %x len %x", td->tag, td->data_len);

	*_xdr = xdr;
	*_toklen = toklen;
	_leave(" = 0 [toklen=%u]", toklen);
	return 0;
}
