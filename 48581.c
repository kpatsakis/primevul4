static int pcrypt_aead_setauthsize(struct crypto_aead *parent,
				   unsigned int authsize)
{
	struct pcrypt_aead_ctx *ctx = crypto_aead_ctx(parent);

	return crypto_aead_setauthsize(ctx->child, authsize);
}
