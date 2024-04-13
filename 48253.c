int crypto_unregister_instance(struct crypto_alg *alg)
{
	int err;
	struct crypto_instance *inst = (void *)alg;
	struct crypto_template *tmpl = inst->tmpl;
	LIST_HEAD(users);

	if (!(alg->cra_flags & CRYPTO_ALG_INSTANCE))
		return -EINVAL;

	BUG_ON(atomic_read(&alg->cra_refcnt) != 1);

	down_write(&crypto_alg_sem);

	hlist_del_init(&inst->list);
	err = crypto_remove_alg(alg, &users);

	up_write(&crypto_alg_sem);

	if (err)
		return err;

	tmpl->free(inst);
	crypto_remove_final(&users);

	return 0;
}
