static void rxrpc_free_preparse_s(struct key_preparsed_payload *prep)
{
	if (prep->payload[0])
		crypto_free_blkcipher(prep->payload[0]);
}
