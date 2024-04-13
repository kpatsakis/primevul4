static void lz4_exit(struct crypto_tfm *tfm)
{
	struct lz4_ctx *ctx = crypto_tfm_ctx(tfm);
	vfree(ctx->lz4_comp_mem);
}
