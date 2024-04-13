static void rd_free_device(struct se_device *dev)
{
	struct rd_dev *rd_dev = RD_DEV(dev);

	rd_release_device_space(rd_dev);
	kfree(rd_dev);
}
