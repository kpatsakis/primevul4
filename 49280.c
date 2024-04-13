int lrw_camellia_setkey(struct crypto_tfm *tfm, const u8 *key,
			unsigned int keylen)
{
	struct camellia_lrw_ctx *ctx = crypto_tfm_ctx(tfm);
	int err;

	err = __camellia_setkey(&ctx->camellia_ctx, key,
				keylen - CAMELLIA_BLOCK_SIZE,
				&tfm->crt_flags);
	if (err)
		return err;

	return lrw_init_table(&ctx->lrw_table,
			      key + keylen - CAMELLIA_BLOCK_SIZE);
}
