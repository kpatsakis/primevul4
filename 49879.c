static int hash_setkey(struct crypto_ahash *tfm,
		       const u8 *key, unsigned int keylen, int alg)
{
	int ret = 0;
	struct hash_ctx *ctx = crypto_ahash_ctx(tfm);

	/**
	 * Freed in final.
	 */
	ctx->key = kmemdup(key, keylen, GFP_KERNEL);
	if (!ctx->key) {
		pr_err("%s: Failed to allocate ctx->key for %d\n",
		       __func__, alg);
		return -ENOMEM;
	}
	ctx->keylen = keylen;

	return ret;
}
