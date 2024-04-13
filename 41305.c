static void __exit tg3_cleanup(void)
{
	pci_unregister_driver(&tg3_driver);
}
