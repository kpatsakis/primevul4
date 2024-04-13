static void __exit md5_sparc64_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
