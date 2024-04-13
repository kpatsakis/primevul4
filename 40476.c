static int setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen)
{
	struct blkcipher_alg *cipher = &tfm->__crt_alg->cra_blkcipher;
	unsigned long alignmask = crypto_tfm_alg_alignmask(tfm);

	if (keylen < cipher->min_keysize || keylen > cipher->max_keysize) {
		tfm->crt_flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	if ((unsigned long)key & alignmask)
		return setkey_unaligned(tfm, key, keylen);

	return cipher->setkey(tfm, key, keylen);
}
