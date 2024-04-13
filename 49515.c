struct crypto_alg *crypto_mod_get(struct crypto_alg *alg)
{
	return try_module_get(alg->cra_module) ? crypto_alg_get(alg) : NULL;
}
