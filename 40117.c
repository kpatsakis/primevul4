static int __init algif_hash_init(void)
{
	return af_alg_register_type(&algif_type_hash);
}
