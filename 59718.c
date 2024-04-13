static void __exit asymmetric_key_cleanup(void)
{
	unregister_key_type(&key_type_asymmetric);
}
