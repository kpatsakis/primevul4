int vhost_log_access_ok(struct vhost_dev *dev)
{
	return memory_access_ok(dev, dev->memory, 1);
}
