static void __exit crc32_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
