static void __exit cleanup_trusted(void)
{
	trusted_shash_release();
	unregister_key_type(&key_type_trusted);
}
