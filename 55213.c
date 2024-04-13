void crypto_unregister_rng(struct rng_alg *alg)
{
	crypto_unregister_alg(&alg->base);
}
