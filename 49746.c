static int __init wp512_mod_init(void)
{
	return crypto_register_shashes(wp_algs, ARRAY_SIZE(wp_algs));
}
