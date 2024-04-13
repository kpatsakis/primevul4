static int __init krng_mod_init(void)
{
	return crypto_register_alg(&krng_alg);
}
