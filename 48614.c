static void vmac_exit_tfm(struct crypto_tfm *tfm)
{
	struct vmac_ctx_t *ctx = crypto_tfm_ctx(tfm);
	crypto_free_cipher(ctx->child);
}
