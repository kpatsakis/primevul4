static int __init tea_mod_init(void)
{
	return crypto_register_algs(tea_algs, ARRAY_SIZE(tea_algs));
}
