static int crypto_ccm_setkey(struct crypto_aead *aead, const u8 *key,
			     unsigned int keylen)
{
	struct crypto_ccm_ctx *ctx = crypto_aead_ctx(aead);
	struct crypto_ablkcipher *ctr = ctx->ctr;
	struct crypto_cipher *tfm = ctx->cipher;
	int err = 0;

	crypto_ablkcipher_clear_flags(ctr, CRYPTO_TFM_REQ_MASK);
	crypto_ablkcipher_set_flags(ctr, crypto_aead_get_flags(aead) &
				    CRYPTO_TFM_REQ_MASK);
	err = crypto_ablkcipher_setkey(ctr, key, keylen);
	crypto_aead_set_flags(aead, crypto_ablkcipher_get_flags(ctr) &
			      CRYPTO_TFM_RES_MASK);
	if (err)
		goto out;

	crypto_cipher_clear_flags(tfm, CRYPTO_TFM_REQ_MASK);
	crypto_cipher_set_flags(tfm, crypto_aead_get_flags(aead) &
				    CRYPTO_TFM_REQ_MASK);
	err = crypto_cipher_setkey(tfm, key, keylen);
	crypto_aead_set_flags(aead, crypto_cipher_get_flags(tfm) &
			      CRYPTO_TFM_RES_MASK);

out:
	return err;
}
