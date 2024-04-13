static void __exit crc32_pclmul_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
