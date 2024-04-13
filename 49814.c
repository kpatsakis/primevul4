static int aes_ablkcipher_setkey(struct crypto_ablkcipher *cipher,
				 const u8 *key, unsigned int keylen)
{
	struct cryp_ctx *ctx = crypto_ablkcipher_ctx(cipher);
	u32 *flags = &cipher->base.crt_flags;

	pr_debug(DEV_DBG_NAME " [%s]", __func__);

	switch (keylen) {
	case AES_KEYSIZE_128:
		ctx->config.keysize = CRYP_KEY_SIZE_128;
		break;

	case AES_KEYSIZE_192:
		ctx->config.keysize = CRYP_KEY_SIZE_192;
		break;

	case AES_KEYSIZE_256:
		ctx->config.keysize = CRYP_KEY_SIZE_256;
		break;

	default:
		pr_err(DEV_DBG_NAME "[%s]: Unknown keylen!", __func__);
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	memcpy(ctx->key, key, keylen);
	ctx->keylen = keylen;

	ctx->updated = 0;

	return 0;
}
