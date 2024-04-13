static bool vfio_pci_nointx(struct pci_dev *pdev)
{
	switch (pdev->vendor) {
	case PCI_VENDOR_ID_INTEL:
		switch (pdev->device) {
		/* All i40e (XL710/X710) 10/20/40GbE NICs */
		case 0x1572:
		case 0x1574:
		case 0x1580 ... 0x1581:
		case 0x1583 ... 0x1589:
		case 0x37d0 ... 0x37d2:
			return true;
		default:
			return false;
		}
	}

	return false;
}
