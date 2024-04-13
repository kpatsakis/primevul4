static int pcrypt_aead_setkey(struct crypto_aead *parent,
			      const u8 *key, unsigned int keylen)
{
	struct pcrypt_aead_ctx *ctx = crypto_aead_ctx(parent);

	return crypto_aead_setkey(ctx->child, key, keylen);
}
