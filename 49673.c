static int __init rmd320_mod_init(void)
{
	return crypto_register_shash(&alg);
}
