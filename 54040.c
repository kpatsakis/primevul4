static ssize_t cuse_class_abort_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	struct cuse_conn *cc = dev_get_drvdata(dev);

	fuse_abort_conn(&cc->fc);
	return count;
}
