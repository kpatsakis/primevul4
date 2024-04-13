static unsigned int crypto_ablkcipher_ctxsize(struct crypto_alg *alg, u32 type,
					      u32 mask)
{
	return alg->cra_ctxsize;
}
