 static int __init crypto_null_mod_init(void)
 {
	int ret = 0;

	ret = crypto_register_algs(null_algs, ARRAY_SIZE(null_algs));
	if (ret < 0)
		goto out;

	ret = crypto_register_shash(&digest_null);
	if (ret < 0)
		goto out_unregister_algs;

	return 0;

out_unregister_algs:
	crypto_unregister_algs(null_algs, ARRAY_SIZE(null_algs));
out:
	return ret;
}
