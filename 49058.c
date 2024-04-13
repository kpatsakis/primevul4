static int __init aes_init(void)
{
	return crypto_register_algs(aes_algs, ARRAY_SIZE(aes_algs));
}
