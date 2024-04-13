static ssize_t vfio_pci_read(void *device_data, char __user *buf,
			     size_t count, loff_t *ppos)
{
	if (!count)
		return 0;

	return vfio_pci_rw(device_data, buf, count, ppos, false);
}
