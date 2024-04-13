static int skcipher_setkey_blkcipher(struct crypto_skcipher *tfm,
				     const u8 *key, unsigned int keylen)
{
	struct crypto_blkcipher **ctx = crypto_skcipher_ctx(tfm);
	struct crypto_blkcipher *blkcipher = *ctx;
	int err;

	crypto_blkcipher_clear_flags(blkcipher, ~0);
	crypto_blkcipher_set_flags(blkcipher, crypto_skcipher_get_flags(tfm) &
					      CRYPTO_TFM_REQ_MASK);
	err = crypto_blkcipher_setkey(blkcipher, key, keylen);
	crypto_skcipher_set_flags(tfm, crypto_blkcipher_get_flags(blkcipher) &
				       CRYPTO_TFM_RES_MASK);

	return err;
}
