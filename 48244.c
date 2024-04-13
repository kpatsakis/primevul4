static int crypto_remove_alg(struct crypto_alg *alg, struct list_head *list)
{
	if (unlikely(list_empty(&alg->cra_list)))
		return -ENOENT;

	alg->cra_flags |= CRYPTO_ALG_DEAD;

	crypto_notify(CRYPTO_MSG_ALG_UNREGISTER, alg);
	list_del_init(&alg->cra_list);
	crypto_remove_spawns(alg, list, NULL);

	return 0;
}
