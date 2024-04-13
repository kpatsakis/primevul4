static int setkey(struct crypto_aead *tfm, const u8 *key, unsigned int keylen)
{
	struct aead_alg *aead = crypto_aead_alg(tfm);
	unsigned long alignmask = crypto_aead_alignmask(tfm);

	if ((unsigned long)key & alignmask)
		return setkey_unaligned(tfm, key, keylen);

	return aead->setkey(tfm, key, keylen);
}
