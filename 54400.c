to_ioeventfd(struct kvm_io_device *dev)
{
	return container_of(dev, struct _ioeventfd, dev);
}
