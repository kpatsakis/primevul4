static int __init cast6_mod_init(void)
{
	return crypto_register_alg(&alg);
}
