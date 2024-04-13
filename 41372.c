static int __init tg3_init(void)
{
	return pci_register_driver(&tg3_driver);
}
