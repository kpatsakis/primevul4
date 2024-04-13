static void __remove(struct device *dev)
{
	struct cros_ec_dev *ec = container_of(dev, struct cros_ec_dev,
					      class_dev);
	kfree(ec);
}
