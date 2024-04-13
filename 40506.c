static unsigned int crypto_rng_ctxsize(struct crypto_alg *alg, u32 type,
				       u32 mask)
{
	return alg->cra_ctxsize;
}
