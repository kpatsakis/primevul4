static int deflate_init(struct crypto_tfm *tfm)
{
	struct deflate_ctx *ctx = crypto_tfm_ctx(tfm);
	int ret;

	ret = deflate_comp_init(ctx);
	if (ret)
		goto out;
	ret = deflate_decomp_init(ctx);
	if (ret)
		deflate_comp_exit(ctx);
out:
	return ret;
}
