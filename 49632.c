static int lzo_init(struct crypto_tfm *tfm)
{
	struct lzo_ctx *ctx = crypto_tfm_ctx(tfm);

	ctx->lzo_comp_mem = kmalloc(LZO1X_MEM_COMPRESS,
				    GFP_KERNEL | __GFP_NOWARN | __GFP_REPEAT);
	if (!ctx->lzo_comp_mem)
		ctx->lzo_comp_mem = vmalloc(LZO1X_MEM_COMPRESS);
	if (!ctx->lzo_comp_mem)
		return -ENOMEM;

	return 0;
}
