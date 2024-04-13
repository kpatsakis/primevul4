static struct crypto_larval *__crypto_register_alg(struct crypto_alg *alg)
{
	struct crypto_alg *q;
	struct crypto_larval *larval;
	int ret = -EAGAIN;

	if (crypto_is_dead(alg))
		goto err;

	INIT_LIST_HEAD(&alg->cra_users);

	/* No cheating! */
	alg->cra_flags &= ~CRYPTO_ALG_TESTED;

	ret = -EEXIST;

	atomic_set(&alg->cra_refcnt, 1);
	list_for_each_entry(q, &crypto_alg_list, cra_list) {
		if (q == alg)
			goto err;

		if (crypto_is_moribund(q))
			continue;

		if (crypto_is_larval(q)) {
			if (!strcmp(alg->cra_driver_name, q->cra_driver_name))
				goto err;
			continue;
		}

		if (!strcmp(q->cra_driver_name, alg->cra_name) ||
		    !strcmp(q->cra_name, alg->cra_driver_name))
			goto err;
	}

	larval = crypto_larval_alloc(alg->cra_name,
				     alg->cra_flags | CRYPTO_ALG_TESTED, 0);
	if (IS_ERR(larval))
		goto out;

	ret = -ENOENT;
	larval->adult = crypto_mod_get(alg);
	if (!larval->adult)
		goto free_larval;

	atomic_set(&larval->alg.cra_refcnt, 1);
	memcpy(larval->alg.cra_driver_name, alg->cra_driver_name,
	       CRYPTO_MAX_ALG_NAME);
	larval->alg.cra_priority = alg->cra_priority;

	list_add(&alg->cra_list, &crypto_alg_list);
	list_add(&larval->alg.cra_list, &crypto_alg_list);

out:
	return larval;

free_larval:
	kfree(larval);
err:
	larval = ERR_PTR(ret);
	goto out;
}
