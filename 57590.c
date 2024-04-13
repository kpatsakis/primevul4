void crypto_unregister_skcipher(struct skcipher_alg *alg)
{
	crypto_unregister_alg(&alg->base);
}
