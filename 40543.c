static int shash_prepare_alg(struct shash_alg *alg)
{
	struct crypto_alg *base = &alg->base;

	if (alg->digestsize > PAGE_SIZE / 8 ||
	    alg->descsize > PAGE_SIZE / 8 ||
	    alg->statesize > PAGE_SIZE / 8)
		return -EINVAL;

	base->cra_type = &crypto_shash_type;
	base->cra_flags &= ~CRYPTO_ALG_TYPE_MASK;
	base->cra_flags |= CRYPTO_ALG_TYPE_SHASH;

	if (!alg->finup)
		alg->finup = shash_finup_unaligned;
	if (!alg->digest)
		alg->digest = shash_digest_unaligned;
	if (!alg->export) {
		alg->export = shash_default_export;
		alg->import = shash_default_import;
		alg->statesize = alg->descsize;
	}
	if (!alg->setkey)
		alg->setkey = shash_no_setkey;

	return 0;
}
