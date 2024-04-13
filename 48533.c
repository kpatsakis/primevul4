struct mcryptd_ahash *mcryptd_alloc_ahash(const char *alg_name,
					u32 type, u32 mask)
{
	char mcryptd_alg_name[CRYPTO_MAX_ALG_NAME];
	struct crypto_ahash *tfm;

	if (snprintf(mcryptd_alg_name, CRYPTO_MAX_ALG_NAME,
		     "mcryptd(%s)", alg_name) >= CRYPTO_MAX_ALG_NAME)
		return ERR_PTR(-EINVAL);
	tfm = crypto_alloc_ahash(mcryptd_alg_name, type, mask);
	if (IS_ERR(tfm))
		return ERR_CAST(tfm);
	if (tfm->base.__crt_alg->cra_module != THIS_MODULE) {
		crypto_free_ahash(tfm);
		return ERR_PTR(-EINVAL);
	}

	return __mcryptd_ahash_cast(tfm);
}
