int crypto_unregister_algs(struct crypto_alg *algs, int count)
{
	int i, ret;

	for (i = 0; i < count; i++) {
		ret = crypto_unregister_alg(&algs[i]);
		if (ret)
			pr_err("Failed to unregister %s %s: %d\n",
			       algs[i].cra_driver_name, algs[i].cra_name, ret);
	}

	return 0;
}
