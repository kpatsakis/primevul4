struct crypto_alg *crypto_lookup_aead(const char *name, u32 type, u32 mask)
{
	struct crypto_alg *alg;

	alg = crypto_alg_mod_lookup(name, type, mask);
	if (IS_ERR(alg))
		return alg;

	if (alg->cra_type == &crypto_aead_type)
		return alg;

	if (!alg->cra_aead.ivsize)
		return alg;

	crypto_mod_put(alg);
	alg = crypto_alg_mod_lookup(name, type | CRYPTO_ALG_TESTED,
				    mask & ~CRYPTO_ALG_TESTED);
	if (IS_ERR(alg))
		return alg;

	if (alg->cra_type == &crypto_aead_type) {
		if ((alg->cra_flags ^ type ^ ~mask) & CRYPTO_ALG_TESTED) {
			crypto_mod_put(alg);
			alg = ERR_PTR(-ENOENT);
		}
		return alg;
	}

	BUG_ON(!alg->cra_aead.ivsize);

	return ERR_PTR(crypto_nivaead_default(alg, type, mask));
}
