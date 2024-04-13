static int aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		       unsigned int key_len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;
	int ret;

	ret = need_fallback(key_len);
	if (ret < 0) {
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	sctx->key_len = key_len;
	if (!ret) {
		memcpy(sctx->key, in_key, key_len);
		return 0;
	}

	return setkey_fallback_cip(tfm, in_key, key_len);
}
