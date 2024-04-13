void crypto_unregister_rngs(struct rng_alg *algs, int count)
{
	int i;

	for (i = count - 1; i >= 0; --i)
		crypto_unregister_rng(algs + i);
}
