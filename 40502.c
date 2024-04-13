int crypto_unregister_pcomp(struct pcomp_alg *alg)
{
	return crypto_unregister_alg(&alg->base);
}
