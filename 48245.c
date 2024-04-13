void crypto_remove_final(struct list_head *list)
{
	struct crypto_alg *alg;
	struct crypto_alg *n;

	list_for_each_entry_safe(alg, n, list, cra_list) {
		list_del_init(&alg->cra_list);
		crypto_alg_put(alg);
	}
}
