static int padlock_sha_import(struct shash_desc *desc, const void *in)
{
	struct padlock_sha_desc *dctx = shash_desc_ctx(desc);
	struct padlock_sha_ctx *ctx = crypto_shash_ctx(desc->tfm);

	dctx->fallback.tfm = ctx->fallback;
	dctx->fallback.flags = desc->flags & CRYPTO_TFM_REQ_MAY_SLEEP;
	return crypto_shash_import(&dctx->fallback, in);
}
