static int setkey(struct crypto_tfm *parent, const u8 *key,
		  unsigned int keylen)
{
	struct priv *ctx = crypto_tfm_ctx(parent);
	struct crypto_cipher *child = ctx->child;
	int err, bsize = LRW_BLOCK_SIZE;
	const u8 *tweak = key + keylen - bsize;

	crypto_cipher_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_cipher_set_flags(child, crypto_tfm_get_flags(parent) &
				       CRYPTO_TFM_REQ_MASK);
	err = crypto_cipher_setkey(child, key, keylen - bsize);
	if (err)
		return err;
	crypto_tfm_set_flags(parent, crypto_cipher_get_flags(child) &
				     CRYPTO_TFM_RES_MASK);

	return lrw_init_table(&ctx->table, tweak);
}
