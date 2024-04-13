static void vfio_pci_remove(struct pci_dev *pdev)
{
	struct vfio_pci_device *vdev;

	vdev = vfio_del_group_dev(&pdev->dev);
	if (!vdev)
		return;

	vfio_iommu_group_put(pdev->dev.iommu_group, &pdev->dev);
	kfree(vdev->region);
	kfree(vdev);

	if (vfio_pci_is_vga(pdev)) {
		vga_client_register(pdev, NULL, NULL, NULL);
		vga_set_legacy_decoding(pdev,
				VGA_RSRC_NORMAL_IO | VGA_RSRC_NORMAL_MEM |
				VGA_RSRC_LEGACY_IO | VGA_RSRC_LEGACY_MEM);
	}

	if (!disable_idle_d3)
		pci_set_power_state(pdev, PCI_D0);
}
