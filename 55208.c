 struct crypto_rng *crypto_alloc_rng(const char *alg_name, u32 type, u32 mask)
 {
	return crypto_alloc_tfm(alg_name, &crypto_rng_type, type, mask);
}
