static int __init khazad_mod_init(void)
{
	int ret = 0;
	
	ret = crypto_register_alg(&khazad_alg);
	return ret;
}
