static int vfio_pci_fill_devs(struct pci_dev *pdev, void *data)
{
	struct vfio_pci_fill_info *fill = data;
	struct iommu_group *iommu_group;

	if (fill->cur == fill->max)
		return -EAGAIN; /* Something changed, try again */

	iommu_group = iommu_group_get(&pdev->dev);
	if (!iommu_group)
		return -EPERM; /* Cannot reset non-isolated devices */

	fill->devices[fill->cur].group_id = iommu_group_id(iommu_group);
	fill->devices[fill->cur].segment = pci_domain_nr(pdev->bus);
	fill->devices[fill->cur].bus = pdev->bus->number;
	fill->devices[fill->cur].devfn = pdev->devfn;
	fill->cur++;
	iommu_group_put(iommu_group);
	return 0;
}
