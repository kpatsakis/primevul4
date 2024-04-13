static int rfc4106_set_key(struct crypto_aead *parent, const u8 *key,
						   unsigned int key_len)
{
	int ret = 0;
	struct crypto_tfm *tfm = crypto_aead_tfm(parent);
	struct aesni_rfc4106_gcm_ctx *ctx = aesni_rfc4106_gcm_ctx_get(parent);
	struct crypto_aead *cryptd_child = cryptd_aead_child(ctx->cryptd_tfm);
	struct aesni_rfc4106_gcm_ctx *child_ctx =
                                 aesni_rfc4106_gcm_ctx_get(cryptd_child);
	u8 *new_key_align, *new_key_mem = NULL;

	if (key_len < 4) {
		crypto_tfm_set_flags(tfm, CRYPTO_TFM_RES_BAD_KEY_LEN);
		return -EINVAL;
	}
	/*Account for 4 byte nonce at the end.*/
	key_len -= 4;
	if (key_len != AES_KEYSIZE_128 && key_len != AES_KEYSIZE_192 &&
	    key_len != AES_KEYSIZE_256) {
		crypto_tfm_set_flags(tfm, CRYPTO_TFM_RES_BAD_KEY_LEN);
		return -EINVAL;
	}

	memcpy(ctx->nonce, key + key_len, sizeof(ctx->nonce));
	/*This must be on a 16 byte boundary!*/
	if ((unsigned long)(&(ctx->aes_key_expanded.key_enc[0])) % AESNI_ALIGN)
		return -EINVAL;

	if ((unsigned long)key % AESNI_ALIGN) {
		/*key is not aligned: use an auxuliar aligned pointer*/
		new_key_mem = kmalloc(key_len+AESNI_ALIGN, GFP_KERNEL);
		if (!new_key_mem)
			return -ENOMEM;

		new_key_align = PTR_ALIGN(new_key_mem, AESNI_ALIGN);
		memcpy(new_key_align, key, key_len);
		key = new_key_align;
	}

	if (!irq_fpu_usable())
		ret = crypto_aes_expand_key(&(ctx->aes_key_expanded),
		key, key_len);
	else {
		kernel_fpu_begin();
		ret = aesni_set_key(&(ctx->aes_key_expanded), key, key_len);
		kernel_fpu_end();
	}
	/*This must be on a 16 byte boundary!*/
	if ((unsigned long)(&(ctx->hash_subkey[0])) % AESNI_ALIGN) {
		ret = -EINVAL;
		goto exit;
	}
	ret = rfc4106_set_hash_subkey(ctx->hash_subkey, key, key_len);
	memcpy(child_ctx, ctx, sizeof(*ctx));
exit:
	kfree(new_key_mem);
	return ret;
}
