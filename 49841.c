static void __exit ux500_cryp_mod_fini(void)
{
	pr_debug("[%s] is called!", __func__);
	platform_driver_unregister(&cryp_driver);
	return;
}
