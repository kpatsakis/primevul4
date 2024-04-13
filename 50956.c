static void _ffs_free_dev(struct ffs_dev *dev)
{
	list_del(&dev->entry);
	if (dev->name_allocated)
		kfree(dev->name);
	kfree(dev);
	if (list_empty(&ffs_devices))
		functionfs_cleanup();
}
