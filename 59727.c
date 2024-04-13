static int __init pkcs7_key_init(void)
{
	return register_key_type(&key_type_pkcs7);
}
