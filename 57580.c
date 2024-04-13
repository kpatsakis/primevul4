struct crypto_skcipher *crypto_alloc_skcipher(const char *alg_name,
					      u32 type, u32 mask)
{
	return crypto_alloc_tfm(alg_name, &crypto_skcipher_type2, type, mask);
}
