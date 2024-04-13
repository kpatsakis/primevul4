static int __init crc32_mod_init(void)
{
	return crypto_register_shash(&alg);
}
