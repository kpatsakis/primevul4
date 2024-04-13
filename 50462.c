static void vfio_pci_disable(struct vfio_pci_device *vdev)
{
	struct pci_dev *pdev = vdev->pdev;
	struct vfio_pci_dummy_resource *dummy_res, *tmp;
	int i, bar;

	/* Stop the device from further DMA */
	pci_clear_master(pdev);

	vfio_pci_set_irqs_ioctl(vdev, VFIO_IRQ_SET_DATA_NONE |
				VFIO_IRQ_SET_ACTION_TRIGGER,
				vdev->irq_type, 0, 0, NULL);

	vdev->virq_disabled = false;

	for (i = 0; i < vdev->num_regions; i++)
		vdev->region[i].ops->release(vdev, &vdev->region[i]);

	vdev->num_regions = 0;
	kfree(vdev->region);
	vdev->region = NULL; /* don't krealloc a freed pointer */

	vfio_config_free(vdev);

	for (bar = PCI_STD_RESOURCES; bar <= PCI_STD_RESOURCE_END; bar++) {
		if (!vdev->barmap[bar])
			continue;
		pci_iounmap(pdev, vdev->barmap[bar]);
		pci_release_selected_regions(pdev, 1 << bar);
		vdev->barmap[bar] = NULL;
	}

	list_for_each_entry_safe(dummy_res, tmp,
				 &vdev->dummy_resources_list, res_next) {
		list_del(&dummy_res->res_next);
		release_resource(&dummy_res->resource);
		kfree(dummy_res);
	}

	vdev->needs_reset = true;

	/*
	 * If we have saved state, restore it.  If we can reset the device,
	 * even better.  Resetting with current state seems better than
	 * nothing, but saving and restoring current state without reset
	 * is just busy work.
	 */
	if (pci_load_and_free_saved_state(pdev, &vdev->pci_saved_state)) {
		pr_info("%s: Couldn't reload %s saved state\n",
			__func__, dev_name(&pdev->dev));

		if (!vdev->reset_works)
			goto out;

		pci_save_state(pdev);
	}

	/*
	 * Disable INTx and MSI, presumably to avoid spurious interrupts
	 * during reset.  Stolen from pci_reset_function()
	 */
	pci_write_config_word(pdev, PCI_COMMAND, PCI_COMMAND_INTX_DISABLE);

	/*
	 * Try to reset the device.  The success of this is dependent on
	 * being able to lock the device, which is not always possible.
	 */
	if (vdev->reset_works && !pci_try_reset_function(pdev))
		vdev->needs_reset = false;

	pci_restore_state(pdev);
out:
	pci_disable_device(pdev);

	vfio_pci_try_bus_reset(vdev);

	if (!disable_idle_d3)
		pci_set_power_state(pdev, PCI_D3hot);
}
