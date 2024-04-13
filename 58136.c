static int crypto_cbcmac_digest_setkey(struct crypto_shash *parent,
				     const u8 *inkey, unsigned int keylen)
{
	struct cbcmac_tfm_ctx *ctx = crypto_shash_ctx(parent);

	return crypto_cipher_setkey(ctx->child, inkey, keylen);
}
