int crypto_register_pcomp(struct pcomp_alg *alg)
{
	struct crypto_alg *base = &alg->base;

	base->cra_type = &crypto_pcomp_type;
	base->cra_flags &= ~CRYPTO_ALG_TYPE_MASK;
	base->cra_flags |= CRYPTO_ALG_TYPE_PCOMPRESS;

	return crypto_register_alg(base);
}
