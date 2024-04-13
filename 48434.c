static int crypto_cts_setkey(struct crypto_tfm *parent, const u8 *key,
			     unsigned int keylen)
{
	struct crypto_cts_ctx *ctx = crypto_tfm_ctx(parent);
	struct crypto_blkcipher *child = ctx->child;
	int err;

	crypto_blkcipher_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_blkcipher_set_flags(child, crypto_tfm_get_flags(parent) &
				       CRYPTO_TFM_REQ_MASK);
	err = crypto_blkcipher_setkey(child, key, keylen);
	crypto_tfm_set_flags(parent, crypto_blkcipher_get_flags(child) &
				     CRYPTO_TFM_RES_MASK);
	return err;
}
