static int __init arc4_init(void)
{
	return crypto_register_algs(arc4_algs, ARRAY_SIZE(arc4_algs));
}
