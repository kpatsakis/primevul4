static int xts_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		       unsigned int key_len)
{
	struct crypto_aes_xts_ctx *ctx = crypto_tfm_ctx(tfm);
	int ret;

	ret = crypto_aes_expand_key(&ctx->key1, in_key, key_len / 2);
	if (!ret)
		ret = crypto_aes_expand_key(&ctx->key2, &in_key[key_len / 2],
					    key_len / 2);
	if (!ret)
		return 0;

	tfm->crt_flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
	return -EINVAL;
}
