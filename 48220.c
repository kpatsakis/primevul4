void crypto_alg_tested(const char *name, int err)
{
	struct crypto_larval *test;
	struct crypto_alg *alg;
	struct crypto_alg *q;
	LIST_HEAD(list);

	down_write(&crypto_alg_sem);
	list_for_each_entry(q, &crypto_alg_list, cra_list) {
		if (crypto_is_moribund(q) || !crypto_is_larval(q))
			continue;

		test = (struct crypto_larval *)q;

		if (!strcmp(q->cra_driver_name, name))
			goto found;
	}

	printk(KERN_ERR "alg: Unexpected test result for %s: %d\n", name, err);
	goto unlock;

found:
	q->cra_flags |= CRYPTO_ALG_DEAD;
	alg = test->adult;
	if (err || list_empty(&alg->cra_list))
		goto complete;

	alg->cra_flags |= CRYPTO_ALG_TESTED;

	list_for_each_entry(q, &crypto_alg_list, cra_list) {
		if (q == alg)
			continue;

		if (crypto_is_moribund(q))
			continue;

		if (crypto_is_larval(q)) {
			struct crypto_larval *larval = (void *)q;

			/*
			 * Check to see if either our generic name or
			 * specific name can satisfy the name requested
			 * by the larval entry q.
			 */
			if (strcmp(alg->cra_name, q->cra_name) &&
			    strcmp(alg->cra_driver_name, q->cra_name))
				continue;

			if (larval->adult)
				continue;
			if ((q->cra_flags ^ alg->cra_flags) & larval->mask)
				continue;
			if (!crypto_mod_get(alg))
				continue;

			larval->adult = alg;
			continue;
		}

		if (strcmp(alg->cra_name, q->cra_name))
			continue;

		if (strcmp(alg->cra_driver_name, q->cra_driver_name) &&
		    q->cra_priority > alg->cra_priority)
			continue;

		crypto_remove_spawns(q, &list, alg);
	}

complete:
	complete_all(&test->completion);

unlock:
	up_write(&crypto_alg_sem);

	crypto_remove_final(&list);
}
