static void crypto_rfc4309_exit_tfm(struct crypto_aead *tfm)
{
	struct crypto_rfc4309_ctx *ctx = crypto_aead_ctx(tfm);

	crypto_free_aead(ctx->child);
}
