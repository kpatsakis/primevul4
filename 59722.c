static int __init asymmetric_key_init(void)
{
	return register_key_type(&key_type_asymmetric);
}
