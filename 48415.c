static void crypto_ctr_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_ctr_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_free_cipher(ctx->child);
}
