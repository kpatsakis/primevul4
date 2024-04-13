static void *ffs_acquire_dev(const char *dev_name)
{
	struct ffs_dev *ffs_dev;

	ENTER();
	ffs_dev_lock();

	ffs_dev = _ffs_find_dev(dev_name);
	if (!ffs_dev)
		ffs_dev = ERR_PTR(-ENOENT);
	else if (ffs_dev->mounted)
		ffs_dev = ERR_PTR(-EBUSY);
	else if (ffs_dev->ffs_acquire_dev_callback &&
	    ffs_dev->ffs_acquire_dev_callback(ffs_dev))
		ffs_dev = ERR_PTR(-ENOENT);
	else
		ffs_dev->mounted = true;

	ffs_dev_unlock();
	return ffs_dev;
}
