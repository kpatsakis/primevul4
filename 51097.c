static void arcmsr_module_exit(void)
{
	pci_unregister_driver(&arcmsr_pci_driver);
}
