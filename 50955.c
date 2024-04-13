static struct ffs_dev *_ffs_do_find_dev(const char *name)
{
	struct ffs_dev *dev;

	list_for_each_entry(dev, &ffs_devices, entry) {
		if (!dev->name || !name)
			continue;
		if (strcmp(dev->name, name) == 0)
			return dev;
	}

	return NULL;
}
