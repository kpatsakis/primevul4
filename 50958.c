static struct ffs_dev *_ffs_get_single_dev(void)
{
	struct ffs_dev *dev;

	if (list_is_singular(&ffs_devices)) {
		dev = list_first_entry(&ffs_devices, struct ffs_dev, entry);
		if (dev->single)
			return dev;
	}

	return NULL;
}
