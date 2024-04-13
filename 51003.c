int ffs_name_dev(struct ffs_dev *dev, const char *name)
{
	int ret;

	ffs_dev_lock();
	ret = _ffs_name_dev(dev, name);
	ffs_dev_unlock();

	return ret;
}
