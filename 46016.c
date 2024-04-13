static ssize_t show_write_buffer_kb(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	ssize_t retval;
	struct comedi_device_file_info *info = dev_get_drvdata(dev);
	unsigned buffer_size_kb = 0;
	struct comedi_subdevice *const write_subdevice =
	    comedi_get_write_subdevice(info);

	mutex_lock(&info->device->mutex);
	if (write_subdevice &&
	    (write_subdevice->subdev_flags & SDF_CMD_WRITE) &&
	    write_subdevice->async) {
		buffer_size_kb = write_subdevice->async->prealloc_bufsz /
		    bytes_per_kibi;
	}
	retval = snprintf(buf, PAGE_SIZE, "%i\n", buffer_size_kb);
	mutex_unlock(&info->device->mutex);

	return retval;
}
