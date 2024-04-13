int lrw_serpent_setkey(struct crypto_tfm *tfm, const u8 *key,
		       unsigned int keylen)
{
	struct serpent_lrw_ctx *ctx = crypto_tfm_ctx(tfm);
	int err;

	err = __serpent_setkey(&ctx->serpent_ctx, key, keylen -
							SERPENT_BLOCK_SIZE);
	if (err)
		return err;

	return lrw_init_table(&ctx->lrw_table, key + keylen -
						SERPENT_BLOCK_SIZE);
}
