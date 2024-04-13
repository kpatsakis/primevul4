void crypto_shoot_alg(struct crypto_alg *alg)
{
	down_write(&crypto_alg_sem);
	alg->cra_flags |= CRYPTO_ALG_DYING;
	up_write(&crypto_alg_sem);
}
