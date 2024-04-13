static int __init crc32c_mod_init(void)
{
	return crypto_register_shash(&alg);
}
