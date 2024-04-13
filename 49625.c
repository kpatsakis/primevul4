static void lz4hc_exit(struct crypto_tfm *tfm)
{
	struct lz4hc_ctx *ctx = crypto_tfm_ctx(tfm);

	vfree(ctx->lz4hc_comp_mem);
}
