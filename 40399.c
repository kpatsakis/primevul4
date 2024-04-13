static int setkey(struct crypto_ablkcipher *tfm, const u8 *key,
		  unsigned int keylen)
{
	struct ablkcipher_alg *cipher = crypto_ablkcipher_alg(tfm);
	unsigned long alignmask = crypto_ablkcipher_alignmask(tfm);

	if (keylen < cipher->min_keysize || keylen > cipher->max_keysize) {
		crypto_ablkcipher_set_flags(tfm, CRYPTO_TFM_RES_BAD_KEY_LEN);
		return -EINVAL;
	}

	if ((unsigned long)key & alignmask)
		return setkey_unaligned(tfm, key, keylen);

	return cipher->setkey(tfm, key, keylen);
}
