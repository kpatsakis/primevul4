static bool vfio_pci_dev_below_slot(struct pci_dev *pdev, struct pci_slot *slot)
{
	for (; pdev; pdev = pdev->bus->self)
		if (pdev->bus == slot->bus)
			return (pdev->slot == slot);
	return false;
}
