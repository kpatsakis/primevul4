static void __exit anubis_mod_fini(void)
{
	crypto_unregister_alg(&anubis_alg);
}
