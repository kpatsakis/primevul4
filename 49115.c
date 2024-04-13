static int des_setkey(struct crypto_tfm *tfm, const u8 *key,
		      unsigned int key_len)
{
	struct s390_des_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;
	u32 tmp[DES_EXPKEY_WORDS];

	/* check for weak keys */
	if (!des_ekey(tmp, key) && (*flags & CRYPTO_TFM_REQ_WEAK_KEY)) {
		*flags |= CRYPTO_TFM_RES_WEAK_KEY;
		return -EINVAL;
	}

	memcpy(ctx->key, key, key_len);
	return 0;
}
