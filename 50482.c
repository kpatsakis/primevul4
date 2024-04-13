static void vfio_pci_try_bus_reset(struct vfio_pci_device *vdev)
{
	struct vfio_devices devs = { .cur_index = 0 };
	int i = 0, ret = -EINVAL;
	bool needs_reset = false, slot = false;
	struct vfio_pci_device *tmp;

	if (!pci_probe_reset_slot(vdev->pdev->slot))
		slot = true;
	else if (pci_probe_reset_bus(vdev->pdev->bus))
		return;

	if (vfio_pci_for_each_slot_or_bus(vdev->pdev, vfio_pci_count_devs,
					  &i, slot) || !i)
		return;

	devs.max_index = i;
	devs.devices = kcalloc(i, sizeof(struct vfio_device *), GFP_KERNEL);
	if (!devs.devices)
		return;

	if (vfio_pci_for_each_slot_or_bus(vdev->pdev,
					  vfio_pci_get_devs, &devs, slot))
		goto put_devs;

	for (i = 0; i < devs.cur_index; i++) {
		tmp = vfio_device_data(devs.devices[i]);
		if (tmp->needs_reset)
			needs_reset = true;
		if (tmp->refcnt)
			goto put_devs;
	}

	if (needs_reset)
		ret = slot ? pci_try_reset_slot(vdev->pdev->slot) :
			     pci_try_reset_bus(vdev->pdev->bus);

put_devs:
	for (i = 0; i < devs.cur_index; i++) {
		tmp = vfio_device_data(devs.devices[i]);
		if (!ret)
			tmp->needs_reset = false;

		if (!tmp->refcnt && !disable_idle_d3)
			pci_set_power_state(tmp->pdev, PCI_D3hot);

		vfio_device_put(devs.devices[i]);
	}

	kfree(devs.devices);
}
