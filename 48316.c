static void crypto_ccm_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_ccm_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_free_cipher(ctx->cipher);
	crypto_free_ablkcipher(ctx->ctr);
}
