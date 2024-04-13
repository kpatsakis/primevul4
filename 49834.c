static int des_ablkcipher_setkey(struct crypto_ablkcipher *cipher,
				 const u8 *key, unsigned int keylen)
{
	struct cryp_ctx *ctx = crypto_ablkcipher_ctx(cipher);
	u32 *flags = &cipher->base.crt_flags;
	u32 tmp[DES_EXPKEY_WORDS];
	int ret;

	pr_debug(DEV_DBG_NAME " [%s]", __func__);
	if (keylen != DES_KEY_SIZE) {
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		pr_debug(DEV_DBG_NAME " [%s]: CRYPTO_TFM_RES_BAD_KEY_LEN",
				__func__);
		return -EINVAL;
	}

	ret = des_ekey(tmp, key);
	if (unlikely(ret == 0) && (*flags & CRYPTO_TFM_REQ_WEAK_KEY)) {
		*flags |= CRYPTO_TFM_RES_WEAK_KEY;
		pr_debug(DEV_DBG_NAME " [%s]: CRYPTO_TFM_REQ_WEAK_KEY",
				__func__);
		return -EINVAL;
	}

	memcpy(ctx->key, key, keylen);
	ctx->keylen = keylen;

	ctx->updated = 0;
	return 0;
}
