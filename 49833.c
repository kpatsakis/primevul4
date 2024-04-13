static int des3_ablkcipher_setkey(struct crypto_ablkcipher *cipher,
				  const u8 *key, unsigned int keylen)
{
	struct cryp_ctx *ctx = crypto_ablkcipher_ctx(cipher);
	u32 *flags = &cipher->base.crt_flags;
	const u32 *K = (const u32 *)key;
	u32 tmp[DES3_EDE_EXPKEY_WORDS];
	int i, ret;

	pr_debug(DEV_DBG_NAME " [%s]", __func__);
	if (keylen != DES3_EDE_KEY_SIZE) {
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		pr_debug(DEV_DBG_NAME " [%s]: CRYPTO_TFM_RES_BAD_KEY_LEN",
				__func__);
		return -EINVAL;
	}

	/* Checking key interdependency for weak key detection. */
	if (unlikely(!((K[0] ^ K[2]) | (K[1] ^ K[3])) ||
				!((K[2] ^ K[4]) | (K[3] ^ K[5]))) &&
			(*flags & CRYPTO_TFM_REQ_WEAK_KEY)) {
		*flags |= CRYPTO_TFM_RES_WEAK_KEY;
		pr_debug(DEV_DBG_NAME " [%s]: CRYPTO_TFM_REQ_WEAK_KEY",
				__func__);
		return -EINVAL;
	}
	for (i = 0; i < 3; i++) {
		ret = des_ekey(tmp, key + i*DES_KEY_SIZE);
		if (unlikely(ret == 0) && (*flags & CRYPTO_TFM_REQ_WEAK_KEY)) {
			*flags |= CRYPTO_TFM_RES_WEAK_KEY;
			pr_debug(DEV_DBG_NAME " [%s]: "
					"CRYPTO_TFM_REQ_WEAK_KEY", __func__);
			return -EINVAL;
		}
	}

	memcpy(ctx->key, key, keylen);
	ctx->keylen = keylen;

	ctx->updated = 0;
	return 0;
}
