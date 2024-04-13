static int __init init_trusted(void)
{
	int ret;

	ret = trusted_shash_alloc();
	if (ret < 0)
		return ret;
	ret = register_key_type(&key_type_trusted);
	if (ret < 0)
		trusted_shash_release();
	return ret;
}
