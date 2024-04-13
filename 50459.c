static void __exit vfio_pci_cleanup(void)
{
	pci_unregister_driver(&vfio_pci_driver);
	vfio_pci_uninit_perm_bits();
}
