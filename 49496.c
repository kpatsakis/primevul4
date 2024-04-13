static struct crypto_alg *__crypto_alg_lookup(const char *name, u32 type,
					      u32 mask)
{
	struct crypto_alg *q, *alg = NULL;
	int best = -2;

	list_for_each_entry(q, &crypto_alg_list, cra_list) {
		int exact, fuzzy;

		if (crypto_is_moribund(q))
			continue;

		if ((q->cra_flags ^ type) & mask)
			continue;

		if (crypto_is_larval(q) &&
		    !crypto_is_test_larval((struct crypto_larval *)q) &&
		    ((struct crypto_larval *)q)->mask != mask)
			continue;

		exact = !strcmp(q->cra_driver_name, name);
		fuzzy = !strcmp(q->cra_name, name);
		if (!exact && !(fuzzy && q->cra_priority > best))
			continue;

		if (unlikely(!crypto_mod_get(q)))
			continue;

		best = q->cra_priority;
		if (alg)
			crypto_mod_put(alg);
		alg = q;

		if (exact)
			break;
	}

	return alg;
}
