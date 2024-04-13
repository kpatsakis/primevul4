static int crypto_rfc4543_setauthsize(struct crypto_aead *parent,
				      unsigned int authsize)
{
	struct crypto_rfc4543_ctx *ctx = crypto_aead_ctx(parent);

	if (authsize != 16)
		return -EINVAL;

	return crypto_aead_setauthsize(ctx->child, authsize);
}
