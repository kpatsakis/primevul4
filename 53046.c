static void __exit atl2_exit_module(void)
{
	pci_unregister_driver(&atl2_driver);
}
