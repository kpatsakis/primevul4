static int hmac_sha1_setkey(struct crypto_ahash *tfm,
			    const u8 *key, unsigned int keylen)
{
	return hash_setkey(tfm, key, keylen, HASH_ALGO_SHA1);
}
