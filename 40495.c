struct crypto_pcomp *crypto_alloc_pcomp(const char *alg_name, u32 type,
					u32 mask)
{
	return crypto_alloc_tfm(alg_name, &crypto_pcomp_type, type, mask);
}
