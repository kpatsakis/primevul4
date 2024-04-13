void *crypto_spawn_tfm2(struct crypto_spawn *spawn)
{
	struct crypto_alg *alg;
	struct crypto_tfm *tfm;

	alg = crypto_spawn_alg(spawn);
	if (IS_ERR(alg))
		return ERR_CAST(alg);

	tfm = crypto_create_tfm(alg, spawn->frontend);
	if (IS_ERR(tfm))
		goto out_put_alg;

	return tfm;

out_put_alg:
	crypto_mod_put(alg);
	return tfm;
}
