static int skcipher_setkey_ablkcipher(struct crypto_skcipher *tfm,
				      const u8 *key, unsigned int keylen)
{
	struct crypto_ablkcipher **ctx = crypto_skcipher_ctx(tfm);
	struct crypto_ablkcipher *ablkcipher = *ctx;
	int err;

	crypto_ablkcipher_clear_flags(ablkcipher, ~0);
	crypto_ablkcipher_set_flags(ablkcipher,
				    crypto_skcipher_get_flags(tfm) &
				    CRYPTO_TFM_REQ_MASK);
	err = crypto_ablkcipher_setkey(ablkcipher, key, keylen);
	crypto_skcipher_set_flags(tfm,
				  crypto_ablkcipher_get_flags(ablkcipher) &
				  CRYPTO_TFM_RES_MASK);

	return err;
}
