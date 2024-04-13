static int lz4hc_init(struct crypto_tfm *tfm)
{
	struct lz4hc_ctx *ctx = crypto_tfm_ctx(tfm);

	ctx->lz4hc_comp_mem = vmalloc(LZ4HC_MEM_COMPRESS);
	if (!ctx->lz4hc_comp_mem)
		return -ENOMEM;

	return 0;
}
