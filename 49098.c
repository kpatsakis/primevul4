static void xts_fallback_exit(struct crypto_tfm *tfm)
{
	struct s390_xts_ctx *xts_ctx = crypto_tfm_ctx(tfm);

	crypto_free_blkcipher(xts_ctx->fallback);
	xts_ctx->fallback = NULL;
}
