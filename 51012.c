int ffs_single_dev(struct ffs_dev *dev)
{
	int ret;

	ret = 0;
	ffs_dev_lock();

	if (!list_is_singular(&ffs_devices))
		ret = -EBUSY;
	else
		dev->single = true;

	ffs_dev_unlock();
	return ret;
}
