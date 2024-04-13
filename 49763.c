static int aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		       unsigned int key_len)
{
	struct aes_ctx *ctx = aes_ctx(tfm);
	const __le32 *key = (const __le32 *)in_key;
	u32 *flags = &tfm->crt_flags;
	struct crypto_aes_ctx gen_aes;
	int cpu;

	if (key_len % 8) {
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	/*
	 * If the hardware is capable of generating the extended key
	 * itself we must supply the plain key for both encryption
	 * and decryption.
	 */
	ctx->D = ctx->E;

	ctx->E[0] = le32_to_cpu(key[0]);
	ctx->E[1] = le32_to_cpu(key[1]);
	ctx->E[2] = le32_to_cpu(key[2]);
	ctx->E[3] = le32_to_cpu(key[3]);

	/* Prepare control words. */
	memset(&ctx->cword, 0, sizeof(ctx->cword));

	ctx->cword.decrypt.encdec = 1;
	ctx->cword.encrypt.rounds = 10 + (key_len - 16) / 4;
	ctx->cword.decrypt.rounds = ctx->cword.encrypt.rounds;
	ctx->cword.encrypt.ksize = (key_len - 16) / 8;
	ctx->cword.decrypt.ksize = ctx->cword.encrypt.ksize;

	/* Don't generate extended keys if the hardware can do it. */
	if (aes_hw_extkey_available(key_len))
		goto ok;

	ctx->D = ctx->d_data;
	ctx->cword.encrypt.keygen = 1;
	ctx->cword.decrypt.keygen = 1;

	if (crypto_aes_expand_key(&gen_aes, in_key, key_len)) {
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	memcpy(ctx->E, gen_aes.key_enc, AES_MAX_KEYLENGTH);
	memcpy(ctx->D, gen_aes.key_dec, AES_MAX_KEYLENGTH);

ok:
	for_each_online_cpu(cpu)
		if (&ctx->cword.encrypt == per_cpu(paes_last_cword, cpu) ||
		    &ctx->cword.decrypt == per_cpu(paes_last_cword, cpu))
			per_cpu(paes_last_cword, cpu) = NULL;

	return 0;
}
