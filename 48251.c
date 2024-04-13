int crypto_unregister_alg(struct crypto_alg *alg)
{
	int ret;
	LIST_HEAD(list);

	down_write(&crypto_alg_sem);
	ret = crypto_remove_alg(alg, &list);
	up_write(&crypto_alg_sem);

	if (ret)
		return ret;

	BUG_ON(atomic_read(&alg->cra_refcnt) != 1);
	if (alg->cra_destroy)
		alg->cra_destroy(alg);

	crypto_remove_final(&list);
	return 0;
}
