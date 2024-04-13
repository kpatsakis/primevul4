static int lrw_cast6_setkey(struct crypto_tfm *tfm, const u8 *key,
			      unsigned int keylen)
{
	struct cast6_lrw_ctx *ctx = crypto_tfm_ctx(tfm);
	int err;

	err = __cast6_setkey(&ctx->cast6_ctx, key, keylen - CAST6_BLOCK_SIZE,
			     &tfm->crt_flags);
	if (err)
		return err;

	return lrw_init_table(&ctx->lrw_table, key + keylen - CAST6_BLOCK_SIZE);
}
