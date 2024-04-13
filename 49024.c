static int aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		unsigned int key_len)
{
	struct AES_CTX *ctx = crypto_tfm_ctx(tfm);

	switch (key_len) {
	case AES_KEYSIZE_128:
		key_len = 128;
		break;
	case AES_KEYSIZE_192:
		key_len = 192;
		break;
	case AES_KEYSIZE_256:
		key_len = 256;
		break;
	default:
		tfm->crt_flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	if (private_AES_set_encrypt_key(in_key, key_len, &ctx->enc_key) == -1) {
		tfm->crt_flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}
	/* private_AES_set_decrypt_key expects an encryption key as input */
	ctx->dec_key = ctx->enc_key;
	if (private_AES_set_decrypt_key(in_key, key_len, &ctx->dec_key) == -1) {
		tfm->crt_flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}
	return 0;
}
