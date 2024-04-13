static void __exit wanxl_cleanup_module(void)
{
	pci_unregister_driver(&wanxl_pci_driver);
}
