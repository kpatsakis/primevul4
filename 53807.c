static void trusted_destroy(struct key *key)
{
	struct trusted_key_payload *p = key->payload.data[0];

	if (!p)
		return;
	memset(p->key, 0, p->key_len);
	kfree(key->payload.data[0]);
}
