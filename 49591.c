static int __init deflate_mod_init(void)
{
	return crypto_register_alg(&alg);
}
