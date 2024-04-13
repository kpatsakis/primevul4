void lrw_twofish_exit_tfm(struct crypto_tfm *tfm)
{
	struct twofish_lrw_ctx *ctx = crypto_tfm_ctx(tfm);

	lrw_free_table(&ctx->lrw_table);
}
