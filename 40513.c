int crypto_init_shash_ops_async(struct crypto_tfm *tfm)
{
	struct crypto_alg *calg = tfm->__crt_alg;
	struct shash_alg *alg = __crypto_shash_alg(calg);
	struct crypto_ahash *crt = __crypto_ahash_cast(tfm);
	struct crypto_shash **ctx = crypto_tfm_ctx(tfm);
	struct crypto_shash *shash;

	if (!crypto_mod_get(calg))
		return -EAGAIN;

	shash = crypto_create_tfm(calg, &crypto_shash_type);
	if (IS_ERR(shash)) {
		crypto_mod_put(calg);
		return PTR_ERR(shash);
	}

	*ctx = shash;
	tfm->exit = crypto_exit_shash_ops_async;

	crt->init = shash_async_init;
	crt->update = shash_async_update;
	crt->final = shash_async_final;
	crt->finup = shash_async_finup;
	crt->digest = shash_async_digest;

	if (alg->setkey)
		crt->setkey = shash_async_setkey;
	if (alg->export)
		crt->export = shash_async_export;
	if (alg->import)
		crt->import = shash_async_import;

	crt->reqsize = sizeof(struct shash_desc) + crypto_shash_descsize(shash);

	return 0;
}
