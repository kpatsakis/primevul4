static unsigned int crypto_shash_extsize(struct crypto_alg *alg)
{
	return alg->cra_ctxsize;
}
