static void __exit md4_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
