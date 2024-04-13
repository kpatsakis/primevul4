static void __exit crc32c_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
