static void __exit algif_skcipher_exit(void)
{
	int err = af_alg_unregister_type(&algif_type_skcipher);
	BUG_ON(err);
}
