static void __exit khazad_mod_fini(void)
{
	crypto_unregister_alg(&khazad_alg);
}
