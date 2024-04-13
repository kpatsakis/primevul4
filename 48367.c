struct cryptd_ablkcipher *cryptd_alloc_ablkcipher(const char *alg_name,
						  u32 type, u32 mask)
{
	char cryptd_alg_name[CRYPTO_MAX_ALG_NAME];
	struct crypto_tfm *tfm;

	if (snprintf(cryptd_alg_name, CRYPTO_MAX_ALG_NAME,
		     "cryptd(%s)", alg_name) >= CRYPTO_MAX_ALG_NAME)
		return ERR_PTR(-EINVAL);
	type &= ~(CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	type |= CRYPTO_ALG_TYPE_BLKCIPHER;
	mask &= ~CRYPTO_ALG_TYPE_MASK;
	mask |= (CRYPTO_ALG_GENIV | CRYPTO_ALG_TYPE_BLKCIPHER_MASK);
	tfm = crypto_alloc_base(cryptd_alg_name, type, mask);
	if (IS_ERR(tfm))
		return ERR_CAST(tfm);
	if (tfm->__crt_alg->cra_module != THIS_MODULE) {
		crypto_free_tfm(tfm);
		return ERR_PTR(-EINVAL);
	}

	return __cryptd_ablkcipher_cast(__crypto_ablkcipher_cast(tfm));
}
