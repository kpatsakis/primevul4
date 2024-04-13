static int vfio_pci_for_each_slot_or_bus(struct pci_dev *pdev,
					 int (*fn)(struct pci_dev *,
						   void *data), void *data,
					 bool slot)
{
	struct vfio_pci_walk_info walk = {
		.fn = fn, .data = data, .pdev = pdev, .slot = slot, .ret = 0,
	};

	pci_walk_bus(pdev->bus, vfio_pci_walk_wrapper, &walk);

	return walk.ret;
}
