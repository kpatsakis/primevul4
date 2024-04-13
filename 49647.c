static void __exit md5_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
