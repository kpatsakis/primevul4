static int cryp_algs_register_all(void)
{
	int ret;
	int i;
	int count;

	pr_debug("[%s]", __func__);

	for (i = 0; i < ARRAY_SIZE(cryp_algs); i++) {
		ret = crypto_register_alg(&cryp_algs[i].crypto);
		if (ret) {
			count = i;
			pr_err("[%s] alg registration failed",
					cryp_algs[i].crypto.cra_driver_name);
			goto unreg;
		}
	}
	return 0;
unreg:
	for (i = 0; i < count; i++)
		crypto_unregister_alg(&cryp_algs[i].crypto);
	return ret;
}
