static int __init rmd128_mod_init(void)
{
	return crypto_register_shash(&alg);
}
