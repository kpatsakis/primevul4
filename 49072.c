static void aes_decrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	const struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

	if (unlikely(need_fallback(sctx->key_len))) {
		crypto_cipher_decrypt_one(sctx->fallback.cip, out, in);
		return;
	}

	switch (sctx->key_len) {
	case 16:
		crypt_s390_km(KM_AES_128_DECRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	case 24:
		crypt_s390_km(KM_AES_192_DECRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	case 32:
		crypt_s390_km(KM_AES_256_DECRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	}
}
