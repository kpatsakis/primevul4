static int skcipher_setkey(void *private, const u8 *key, unsigned int keylen)
{
	return crypto_ablkcipher_setkey(private, key, keylen);
}
