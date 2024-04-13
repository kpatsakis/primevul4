static void __exit sha1_sparc64_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
