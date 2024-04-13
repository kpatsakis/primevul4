static int vfio_pci_enable(struct vfio_pci_device *vdev)
{
	struct pci_dev *pdev = vdev->pdev;
	int ret;
	u16 cmd;
	u8 msix_pos;

	pci_set_power_state(pdev, PCI_D0);

	/* Don't allow our initial saved state to include busmaster */
	pci_clear_master(pdev);

	ret = pci_enable_device(pdev);
	if (ret)
		return ret;

	vdev->reset_works = (pci_reset_function(pdev) == 0);
	pci_save_state(pdev);
	vdev->pci_saved_state = pci_store_saved_state(pdev);
	if (!vdev->pci_saved_state)
		pr_debug("%s: Couldn't store %s saved state\n",
			 __func__, dev_name(&pdev->dev));

	if (likely(!nointxmask)) {
		if (vfio_pci_nointx(pdev)) {
			dev_info(&pdev->dev, "Masking broken INTx support\n");
			vdev->nointx = true;
			pci_intx(pdev, 0);
		} else
			vdev->pci_2_3 = pci_intx_mask_supported(pdev);
	}

	pci_read_config_word(pdev, PCI_COMMAND, &cmd);
	if (vdev->pci_2_3 && (cmd & PCI_COMMAND_INTX_DISABLE)) {
		cmd &= ~PCI_COMMAND_INTX_DISABLE;
		pci_write_config_word(pdev, PCI_COMMAND, cmd);
	}

	ret = vfio_config_init(vdev);
	if (ret) {
		kfree(vdev->pci_saved_state);
		vdev->pci_saved_state = NULL;
		pci_disable_device(pdev);
		return ret;
	}

	msix_pos = pdev->msix_cap;
	if (msix_pos) {
		u16 flags;
		u32 table;

		pci_read_config_word(pdev, msix_pos + PCI_MSIX_FLAGS, &flags);
		pci_read_config_dword(pdev, msix_pos + PCI_MSIX_TABLE, &table);

		vdev->msix_bar = table & PCI_MSIX_TABLE_BIR;
		vdev->msix_offset = table & PCI_MSIX_TABLE_OFFSET;
		vdev->msix_size = ((flags & PCI_MSIX_FLAGS_QSIZE) + 1) * 16;
	} else
		vdev->msix_bar = 0xFF;

	if (!vfio_vga_disabled() && vfio_pci_is_vga(pdev))
		vdev->has_vga = true;


	if (vfio_pci_is_vga(pdev) &&
	    pdev->vendor == PCI_VENDOR_ID_INTEL &&
	    IS_ENABLED(CONFIG_VFIO_PCI_IGD)) {
		ret = vfio_pci_igd_init(vdev);
		if (ret) {
			dev_warn(&vdev->pdev->dev,
				 "Failed to setup Intel IGD regions\n");
			vfio_pci_disable(vdev);
			return ret;
		}
	}

	vfio_pci_probe_mmaps(vdev);

	return 0;
}
