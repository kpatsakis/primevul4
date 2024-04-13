 static int __init des_generic_mod_init(void)
 {
	return crypto_register_algs(des_algs, ARRAY_SIZE(des_algs));
}
