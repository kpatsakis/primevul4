static void rxrpc_destroy(struct key *key)
{
	rxrpc_free_token_list(key->payload.data);
}
