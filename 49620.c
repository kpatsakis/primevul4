static int lz4_init(struct crypto_tfm *tfm)
{
	struct lz4_ctx *ctx = crypto_tfm_ctx(tfm);

	ctx->lz4_comp_mem = vmalloc(LZ4_MEM_COMPRESS);
	if (!ctx->lz4_comp_mem)
		return -ENOMEM;

	return 0;
}
