static void __exit sha1_powerpc_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
