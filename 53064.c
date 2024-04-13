static int __init atl2_init_module(void)
{
	printk(KERN_INFO "%s - version %s\n", atl2_driver_string,
		atl2_driver_version);
	printk(KERN_INFO "%s\n", atl2_copyright);
	return pci_register_driver(&atl2_driver);
}
