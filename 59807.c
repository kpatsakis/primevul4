static void rxrpc_destroy_s(struct key *key)
{
	if (key->payload.data) {
		crypto_free_blkcipher(key->payload.data);
		key->payload.data = NULL;
	}
}
