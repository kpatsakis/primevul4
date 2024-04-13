static void __exit sha512_sparc64_mod_fini(void)
{
	crypto_unregister_shash(&sha384);
	crypto_unregister_shash(&sha512);
}
