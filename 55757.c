int tcp_md5_hash_key(struct tcp_md5sig_pool *hp, const struct tcp_md5sig_key *key)
{
	struct scatterlist sg;

	sg_init_one(&sg, key->key, key->keylen);
	ahash_request_set_crypt(hp->md5_req, &sg, NULL, key->keylen);
	return crypto_ahash_update(hp->md5_req);
}
