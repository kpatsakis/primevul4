void crypto_larval_kill(struct crypto_alg *alg)
{
	struct crypto_larval *larval = (void *)alg;

	down_write(&crypto_alg_sem);
	list_del(&alg->cra_list);
	up_write(&crypto_alg_sem);
	complete_all(&larval->completion);
	crypto_alg_put(alg);
}
