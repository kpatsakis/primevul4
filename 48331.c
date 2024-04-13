static int crypto_rfc4309_setkey(struct crypto_aead *parent, const u8 *key,
				 unsigned int keylen)
{
	struct crypto_rfc4309_ctx *ctx = crypto_aead_ctx(parent);
	struct crypto_aead *child = ctx->child;
	int err;

	if (keylen < 3)
		return -EINVAL;

	keylen -= 3;
	memcpy(ctx->nonce, key + keylen, 3);

	crypto_aead_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_aead_set_flags(child, crypto_aead_get_flags(parent) &
				     CRYPTO_TFM_REQ_MASK);
	err = crypto_aead_setkey(child, key, keylen);
	crypto_aead_set_flags(parent, crypto_aead_get_flags(child) &
				      CRYPTO_TFM_RES_MASK);

	return err;
}
