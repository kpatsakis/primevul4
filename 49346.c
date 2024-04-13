static void __exit crct10dif_intel_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
