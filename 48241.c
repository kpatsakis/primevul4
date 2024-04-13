int crypto_register_instance(struct crypto_template *tmpl,
			     struct crypto_instance *inst)
{
	struct crypto_larval *larval;
	int err;

	err = crypto_check_alg(&inst->alg);
	if (err)
		goto err;

	inst->alg.cra_module = tmpl->module;
	inst->alg.cra_flags |= CRYPTO_ALG_INSTANCE;

	down_write(&crypto_alg_sem);

	larval = __crypto_register_alg(&inst->alg);
	if (IS_ERR(larval))
		goto unlock;

	hlist_add_head(&inst->list, &tmpl->instances);
	inst->tmpl = tmpl;

unlock:
	up_write(&crypto_alg_sem);

	err = PTR_ERR(larval);
	if (IS_ERR(larval))
		goto err;

	crypto_wait_for_test(larval);
	err = 0;

err:
	return err;
}
