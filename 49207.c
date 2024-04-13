static void __exit sha256_sparc64_mod_fini(void)
{
	crypto_unregister_shash(&sha224);
	crypto_unregister_shash(&sha256);
}
