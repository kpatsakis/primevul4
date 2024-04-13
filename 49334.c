static void __exit crc32c_intel_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
