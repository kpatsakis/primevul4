static void padlock_cra_exit(struct crypto_tfm *tfm)
{
	struct padlock_sha_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_free_shash(ctx->fallback);
}
