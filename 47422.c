static int lrw_aesni_setkey(struct crypto_tfm *tfm, const u8 *key,
			    unsigned int keylen)
{
	struct aesni_lrw_ctx *ctx = crypto_tfm_ctx(tfm);
	int err;

	err = aes_set_key_common(tfm, ctx->raw_aes_ctx, key,
				 keylen - AES_BLOCK_SIZE);
	if (err)
		return err;

	return lrw_init_table(&ctx->lrw_table, key + keylen - AES_BLOCK_SIZE);
}
