static int mcryptd_hash_setkey(struct crypto_ahash *parent,
				   const u8 *key, unsigned int keylen)
{
	struct mcryptd_hash_ctx *ctx   = crypto_ahash_ctx(parent);
	struct crypto_shash *child = ctx->child;
	int err;

	crypto_shash_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_shash_set_flags(child, crypto_ahash_get_flags(parent) &
				      CRYPTO_TFM_REQ_MASK);
	err = crypto_shash_setkey(child, key, keylen);
	crypto_ahash_set_flags(parent, crypto_shash_get_flags(child) &
				       CRYPTO_TFM_RES_MASK);
	return err;
}
