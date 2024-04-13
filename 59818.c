static int rxrpc_preparse_s(struct key_preparsed_payload *prep)
{
	struct crypto_blkcipher *ci;

	_enter("%zu", prep->datalen);

	if (prep->datalen != 8)
		return -EINVAL;

	memcpy(&prep->type_data, prep->data, 8);

	ci = crypto_alloc_blkcipher("pcbc(des)", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(ci)) {
		_leave(" = %ld", PTR_ERR(ci));
		return PTR_ERR(ci);
	}

	if (crypto_blkcipher_setkey(ci, prep->data, 8) < 0)
		BUG();

	prep->payload[0] = ci;
	_leave(" = 0");
	return 0;
}
