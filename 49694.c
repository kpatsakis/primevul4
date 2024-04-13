int serpent_setkey(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen)
{
	return __serpent_setkey(crypto_tfm_ctx(tfm), key, keylen);
}
