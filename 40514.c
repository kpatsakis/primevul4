static int crypto_init_shash_ops_compat(struct crypto_tfm *tfm)
{
	struct hash_tfm *crt = &tfm->crt_hash;
	struct crypto_alg *calg = tfm->__crt_alg;
	struct shash_alg *alg = __crypto_shash_alg(calg);
	struct shash_desc **descp = crypto_tfm_ctx(tfm);
	struct crypto_shash *shash;
	struct shash_desc *desc;

	if (!crypto_mod_get(calg))
		return -EAGAIN;

	shash = crypto_create_tfm(calg, &crypto_shash_type);
	if (IS_ERR(shash)) {
		crypto_mod_put(calg);
		return PTR_ERR(shash);
	}

	desc = kmalloc(sizeof(*desc) + crypto_shash_descsize(shash),
		       GFP_KERNEL);
	if (!desc) {
		crypto_free_shash(shash);
		return -ENOMEM;
	}

	*descp = desc;
	desc->tfm = shash;
	tfm->exit = crypto_exit_shash_ops_compat;

	crt->init = shash_compat_init;
	crt->update = shash_compat_update;
	crt->final  = shash_compat_final;
	crt->digest = shash_compat_digest;
	crt->setkey = shash_compat_setkey;

	crt->digestsize = alg->digestsize;

	return 0;
}
