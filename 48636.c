static int init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_cipher *cipher;
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct crypto_spawn *spawn = crypto_instance_ctx(inst);
	struct priv *ctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;

	cipher = crypto_spawn_cipher(spawn);
	if (IS_ERR(cipher))
		return PTR_ERR(cipher);

	if (crypto_cipher_blocksize(cipher) != XTS_BLOCK_SIZE) {
		*flags |= CRYPTO_TFM_RES_BAD_BLOCK_LEN;
		crypto_free_cipher(cipher);
		return -EINVAL;
	}

	ctx->child = cipher;

	cipher = crypto_spawn_cipher(spawn);
	if (IS_ERR(cipher)) {
		crypto_free_cipher(ctx->child);
		return PTR_ERR(cipher);
	}

	/* this check isn't really needed, leave it here just in case */
	if (crypto_cipher_blocksize(cipher) != XTS_BLOCK_SIZE) {
		crypto_free_cipher(cipher);
		crypto_free_cipher(ctx->child);
		*flags |= CRYPTO_TFM_RES_BAD_BLOCK_LEN;
		return -EINVAL;
	}

	ctx->tweak = cipher;

	return 0;
}
