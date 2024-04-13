static int crypto_ahash_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_ahash *hash = __crypto_ahash_cast(tfm);
	struct ahash_alg *alg = crypto_ahash_alg(hash);

	hash->setkey = ahash_nosetkey;
	hash->export = ahash_no_export;
	hash->import = ahash_no_import;

	if (tfm->__crt_alg->cra_type != &crypto_ahash_type)
		return crypto_init_shash_ops_async(tfm);

	hash->init = alg->init;
	hash->update = alg->update;
	hash->final = alg->final;
	hash->finup = alg->finup ?: ahash_def_finup;
	hash->digest = alg->digest;

	if (alg->setkey)
		hash->setkey = alg->setkey;
	if (alg->export)
		hash->export = alg->export;
	if (alg->import)
		hash->import = alg->import;

	return 0;
}
