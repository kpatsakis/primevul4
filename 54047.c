static void cuse_gendev_release(struct device *dev)
{
	kfree(dev);
}
