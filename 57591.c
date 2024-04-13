void crypto_unregister_skciphers(struct skcipher_alg *algs, int count)
{
	int i;

	for (i = count - 1; i >= 0; --i)
		crypto_unregister_skcipher(&algs[i]);
}
