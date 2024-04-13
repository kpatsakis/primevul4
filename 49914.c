static void ap_device_release(struct device *dev)
{
	struct ap_device *ap_dev = to_ap_dev(dev);

	kfree(ap_dev);
}
