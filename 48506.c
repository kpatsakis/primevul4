static void hmac_exit_tfm(struct crypto_tfm *tfm)
{
	struct hmac_ctx *ctx = hmac_ctx(__crypto_shash_cast(tfm));
	crypto_free_shash(ctx->hash);
}
