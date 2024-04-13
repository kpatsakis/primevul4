static int crypto_rfc3686_setkey(struct crypto_ablkcipher *parent,
				 const u8 *key, unsigned int keylen)
{
	struct crypto_rfc3686_ctx *ctx = crypto_ablkcipher_ctx(parent);
	struct crypto_ablkcipher *child = ctx->child;
	int err;

	/* the nonce is stored in bytes at end of key */
	if (keylen < CTR_RFC3686_NONCE_SIZE)
		return -EINVAL;

	memcpy(ctx->nonce, key + (keylen - CTR_RFC3686_NONCE_SIZE),
	       CTR_RFC3686_NONCE_SIZE);

	keylen -= CTR_RFC3686_NONCE_SIZE;

	crypto_ablkcipher_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_ablkcipher_set_flags(child, crypto_ablkcipher_get_flags(parent) &
				    CRYPTO_TFM_REQ_MASK);
	err = crypto_ablkcipher_setkey(child, key, keylen);
	crypto_ablkcipher_set_flags(parent, crypto_ablkcipher_get_flags(child) &
				    CRYPTO_TFM_RES_MASK);

	return err;
}
