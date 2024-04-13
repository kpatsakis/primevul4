static void ffs_release_dev(struct ffs_data *ffs_data)
{
	struct ffs_dev *ffs_dev;

	ENTER();
	ffs_dev_lock();

	ffs_dev = ffs_data->private_data;
	if (ffs_dev) {
		ffs_dev->mounted = false;

		if (ffs_dev->ffs_release_dev_callback)
			ffs_dev->ffs_release_dev_callback(ffs_dev);
	}

	ffs_dev_unlock();
}
