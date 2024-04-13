static void xcbc_exit_tfm(struct crypto_tfm *tfm)
{
	struct xcbc_tfm_ctx *ctx = crypto_tfm_ctx(tfm);
	crypto_free_cipher(ctx->child);
}
