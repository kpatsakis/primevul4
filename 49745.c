static void __exit wp512_mod_fini(void)
{
 	crypto_unregister_shashes(wp_algs, ARRAY_SIZE(wp_algs));
 }
