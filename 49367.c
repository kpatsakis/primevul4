static int __init ghash_pclmulqdqni_mod_init(void)
{
	int err;

	if (!x86_match_cpu(pcmul_cpu_id))
		return -ENODEV;

	err = crypto_register_shash(&ghash_alg);
	if (err)
		goto err_out;
	err = crypto_register_ahash(&ghash_async_alg);
	if (err)
		goto err_shash;

	return 0;

err_shash:
	crypto_unregister_shash(&ghash_alg);
err_out:
	return err;
}
