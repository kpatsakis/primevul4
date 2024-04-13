static void cryp_algs_unregister_all(void)
{
	int i;

	pr_debug(DEV_DBG_NAME " [%s]", __func__);

	for (i = 0; i < ARRAY_SIZE(cryp_algs); i++)
		crypto_unregister_alg(&cryp_algs[i].crypto);
}
