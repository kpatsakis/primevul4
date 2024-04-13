int crypto_unregister_shashes(struct shash_alg *algs, int count)
{
	int i, ret;

	for (i = count - 1; i >= 0; --i) {
		ret = crypto_unregister_shash(&algs[i]);
		if (ret)
			pr_err("Failed to unregister %s %s: %d\n",
			       algs[i].base.cra_driver_name,
			       algs[i].base.cra_name, ret);
	}

	return 0;
}
