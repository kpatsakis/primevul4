static int __init md5_mod_init(void)
{
	return crypto_register_shash(&alg);
}
