static void __exit ux500_hash_mod_fini(void)
{
	platform_driver_unregister(&hash_driver);
}
