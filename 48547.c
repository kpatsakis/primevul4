static void mcryptd_hash_exit_tfm(struct crypto_tfm *tfm)
{
	struct mcryptd_hash_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_free_shash(ctx->child);
}
