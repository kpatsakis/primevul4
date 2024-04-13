static void deflate_exit(struct crypto_tfm *tfm)
{
	struct deflate_ctx *ctx = crypto_tfm_ctx(tfm);

	deflate_comp_exit(ctx);
	deflate_decomp_exit(ctx);
}
