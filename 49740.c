static int __init twofish_mod_init(void)
{
	return crypto_register_alg(&alg);
}
