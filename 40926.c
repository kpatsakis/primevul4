int tcp_md5_hash_key(struct tcp_md5sig_pool *hp, struct tcp_md5sig_key *key)
{
	struct scatterlist sg;

	sg_init_one(&sg, key->key, key->keylen);
	return crypto_hash_update(&hp->md5_desc, &sg, key->keylen);
}
