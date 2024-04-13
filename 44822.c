fst_cleanup_module(void)
{
	pr_info("FarSync WAN driver unloading\n");
	pci_unregister_driver(&fst_driver);
}
