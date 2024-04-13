static void crypto_remove_spawn(struct crypto_spawn *spawn,
				struct list_head *list)
{
	struct crypto_instance *inst = spawn->inst;
	struct crypto_template *tmpl = inst->tmpl;

	if (crypto_is_dead(&inst->alg))
		return;

	inst->alg.cra_flags |= CRYPTO_ALG_DEAD;
	if (hlist_unhashed(&inst->list))
		return;

	if (!tmpl || !crypto_tmpl_get(tmpl))
		return;

	crypto_notify(CRYPTO_MSG_ALG_UNREGISTER, &inst->alg);
	list_move(&inst->alg.cra_list, list);
	hlist_del(&inst->list);
	inst->alg.cra_destroy = crypto_destroy_instance;

	BUG_ON(!list_empty(&inst->alg.cra_users));
}
