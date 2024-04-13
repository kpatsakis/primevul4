static int aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		       unsigned int key_len)
{
	struct crypto_sparc64_aes_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;

	switch (key_len) {
	case AES_KEYSIZE_128:
		ctx->expanded_key_length = 0xb0;
		ctx->ops = &aes128_ops;
		break;

	case AES_KEYSIZE_192:
		ctx->expanded_key_length = 0xd0;
		ctx->ops = &aes192_ops;
		break;

	case AES_KEYSIZE_256:
		ctx->expanded_key_length = 0xf0;
		ctx->ops = &aes256_ops;
		break;

	default:
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	aes_sparc64_key_expand((const u32 *)in_key, &ctx->key[0], key_len);
	ctx->key_length = key_len;

	return 0;
}
