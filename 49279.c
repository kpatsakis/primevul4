void lrw_camellia_exit_tfm(struct crypto_tfm *tfm)
{
	struct camellia_lrw_ctx *ctx = crypto_tfm_ctx(tfm);

	lrw_free_table(&ctx->lrw_table);
}
