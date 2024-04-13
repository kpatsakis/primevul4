static ssize_t store_read_buffer_kb(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	struct comedi_device_file_info *info = dev_get_drvdata(dev);
	unsigned long new_size_kb;
	uint64_t new_size;
	int retval;
	struct comedi_subdevice *const read_subdevice =
	    comedi_get_read_subdevice(info);

	if (strict_strtoul(buf, 10, &new_size_kb))
		return -EINVAL;
	if (new_size_kb != (uint32_t) new_size_kb)
		return -EINVAL;
	new_size = ((uint64_t) new_size_kb) * bytes_per_kibi;
	if (new_size != (uint32_t) new_size)
		return -EINVAL;

	mutex_lock(&info->device->mutex);
	if (read_subdevice == NULL ||
	    (read_subdevice->subdev_flags & SDF_CMD_READ) == 0 ||
	    read_subdevice->async == NULL) {
		mutex_unlock(&info->device->mutex);
		return -EINVAL;
	}
	retval = resize_async_buffer(info->device, read_subdevice,
				     read_subdevice->async, new_size);
	mutex_unlock(&info->device->mutex);

	if (retval < 0)
		return retval;
	return count;
}
