static ssize_t floppy_cmos_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct platform_device *p = to_platform_device(dev);
	int drive;

	drive = p->id;
	return sprintf(buf, "%X\n", UDP->cmos);
}
