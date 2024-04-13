static void cryptd_aead_exit_tfm(struct crypto_tfm *tfm)
{
	struct cryptd_aead_ctx *ctx = crypto_tfm_ctx(tfm);
	crypto_free_aead(ctx->child);
}
