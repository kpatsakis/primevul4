struct crypto_aead *cryptd_aead_child(struct cryptd_aead *tfm)
{
	struct cryptd_aead_ctx *ctx;
	ctx = crypto_aead_ctx(&tfm->base);
	return ctx->child;
}
