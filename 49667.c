static int __init rmd256_mod_init(void)
{
	return crypto_register_shash(&alg);
}
