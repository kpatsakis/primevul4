int crypto_unregister_ahash(struct ahash_alg *alg)
{
	return crypto_unregister_alg(&alg->halg.base);
}
