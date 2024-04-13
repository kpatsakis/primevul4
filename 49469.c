static int nx842_init(struct crypto_tfm *tfm)
{
	struct nx842_ctx *ctx = crypto_tfm_ctx(tfm);
	int wmemsize;

	wmemsize = max_t(int, nx842_get_workmem_size(), LZO1X_MEM_COMPRESS);
	ctx->nx842_wmem = kmalloc(wmemsize, GFP_NOFS);
	if (!ctx->nx842_wmem)
		return -ENOMEM;

	return 0;
}
