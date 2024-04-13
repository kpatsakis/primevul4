void crypto_unregister_template(struct crypto_template *tmpl)
{
	struct crypto_instance *inst;
	struct hlist_node *n;
	struct hlist_head *list;
	LIST_HEAD(users);

	down_write(&crypto_alg_sem);

	BUG_ON(list_empty(&tmpl->list));
	list_del_init(&tmpl->list);

	list = &tmpl->instances;
	hlist_for_each_entry(inst, list, list) {
		int err = crypto_remove_alg(&inst->alg, &users);
		BUG_ON(err);
	}

	crypto_notify(CRYPTO_MSG_TMPL_UNREGISTER, tmpl);

	up_write(&crypto_alg_sem);

	hlist_for_each_entry_safe(inst, n, list, list) {
		BUG_ON(atomic_read(&inst->alg.cra_refcnt) != 1);
		tmpl->free(inst);
	}
	crypto_remove_final(&users);
}
