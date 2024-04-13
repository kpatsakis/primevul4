int xts_camellia_setkey(struct crypto_tfm *tfm, const u8 *key,
			unsigned int keylen)
{
	struct camellia_xts_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;
	int err;

	/* key consists of keys of equal size concatenated, therefore
	 * the length must be even
	 */
	if (keylen % 2) {
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	/* first half of xts-key is for crypt */
	err = __camellia_setkey(&ctx->crypt_ctx, key, keylen / 2, flags);
	if (err)
		return err;

	/* second half of xts-key is for tweak */
	return __camellia_setkey(&ctx->tweak_ctx, key + keylen / 2, keylen / 2,
				flags);
}
