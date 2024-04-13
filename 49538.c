static int __init cast5_mod_init(void)
{
	return crypto_register_alg(&alg);
}
