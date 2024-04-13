int crypto_aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		unsigned int key_len)
{
	struct crypto_aes_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;
	int ret;

	ret = crypto_aes_expand_key(ctx, in_key, key_len);
	if (!ret)
		return 0;

	*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
	return -EINVAL;
}
