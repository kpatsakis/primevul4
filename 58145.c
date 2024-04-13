static void crypto_ccm_exit_tfm(struct crypto_aead *tfm)
{
	struct crypto_ccm_ctx *ctx = crypto_aead_ctx(tfm);

	crypto_free_ahash(ctx->mac);
	crypto_free_skcipher(ctx->ctr);
}
