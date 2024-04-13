static void __exit sha1_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
