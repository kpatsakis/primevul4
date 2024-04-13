static int ghash_final(struct shash_desc *desc, u8 *dst)
{
	struct ghash_desc_ctx *dctx = shash_desc_ctx(desc);
	struct ghash_ctx *ctx = crypto_shash_ctx(desc->tfm);
	int ret;

	ret = ghash_flush(ctx, dctx);
	if (!ret)
		memcpy(dst, ctx->icv, GHASH_BLOCK_SIZE);
	return ret;
}
