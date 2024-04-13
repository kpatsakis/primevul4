static int vfio_pci_count_devs(struct pci_dev *pdev, void *data)
{
	(*(int *)data)++;
	return 0;
}
