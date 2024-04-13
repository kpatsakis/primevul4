static void __exit tea_mod_fini(void)
{
 	crypto_unregister_algs(tea_algs, ARRAY_SIZE(tea_algs));
 }
