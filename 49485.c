static void free_prng_context(struct prng_context *ctx)
{
	crypto_free_cipher(ctx->tfm);
}
