int lrw_twofish_setkey(struct crypto_tfm *tfm, const u8 *key,
		       unsigned int keylen)
{
	struct twofish_lrw_ctx *ctx = crypto_tfm_ctx(tfm);
	int err;

	err = __twofish_setkey(&ctx->twofish_ctx, key, keylen - TF_BLOCK_SIZE,
			       &tfm->crt_flags);
	if (err)
		return err;

	return lrw_init_table(&ctx->lrw_table, key + keylen - TF_BLOCK_SIZE);
}
