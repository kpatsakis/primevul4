static int __init md4_mod_init(void)
{
	return crypto_register_shash(&alg);
}
