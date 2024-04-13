static int crypto_init_rng_ops(struct crypto_tfm *tfm, u32 type, u32 mask)
{
	struct rng_alg *alg = &tfm->__crt_alg->cra_rng;
	struct rng_tfm *ops = &tfm->crt_rng;

	ops->rng_gen_random = alg->rng_make_random;
	ops->rng_reset = rngapi_reset;

	return 0;
}
