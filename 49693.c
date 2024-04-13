static int __init serpent_mod_init(void)
{
	return crypto_register_algs(srp_algs, ARRAY_SIZE(srp_algs));
}
