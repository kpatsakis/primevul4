static ssize_t cuse_class_waiting_show(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	struct cuse_conn *cc = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", atomic_read(&cc->fc.num_waiting));
}
