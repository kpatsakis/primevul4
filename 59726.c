static void __exit pkcs7_key_cleanup(void)
{
	unregister_key_type(&key_type_pkcs7);
}
