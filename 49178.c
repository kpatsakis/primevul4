static int des_set_key(struct crypto_tfm *tfm, const u8 *key,
		       unsigned int keylen)
{
	struct des_sparc64_ctx *dctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;
	u32 tmp[DES_EXPKEY_WORDS];
	int ret;

	/* Even though we have special instructions for key expansion,
	 * we call des_ekey() so that we don't have to write our own
	 * weak key detection code.
	 */
	ret = des_ekey(tmp, key);
	if (unlikely(ret == 0) && (*flags & CRYPTO_TFM_REQ_WEAK_KEY)) {
		*flags |= CRYPTO_TFM_RES_WEAK_KEY;
		return -EINVAL;
	}

	des_sparc64_key_expand((const u32 *) key, &dctx->encrypt_expkey[0]);
	encrypt_to_decrypt(&dctx->decrypt_expkey[0], &dctx->encrypt_expkey[0]);

	return 0;
}
