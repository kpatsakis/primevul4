static void crypto_authenc_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_authenc_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_free_ahash(ctx->auth);
	crypto_free_ablkcipher(ctx->enc);
}
