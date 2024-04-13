static int comedi_close(struct inode *inode, struct file *file)
{
	const unsigned minor = iminor(inode);
	struct comedi_device_file_info *dev_file_info =
	    comedi_get_device_file_info(minor);
	struct comedi_device *dev = dev_file_info->device;
	struct comedi_subdevice *s = NULL;
	int i;

	mutex_lock(&dev->mutex);

	if (dev->subdevices) {
		for (i = 0; i < dev->n_subdevices; i++) {
			s = dev->subdevices + i;

			if (s->busy == file)
				do_cancel(dev, s);
			if (s->lock == file)
				s->lock = NULL;
		}
	}
	if (dev->attached && dev->use_count == 1 && dev->close)
		dev->close(dev);

	module_put(THIS_MODULE);
	if (dev->attached)
		module_put(dev->driver->module);

	dev->use_count--;

	mutex_unlock(&dev->mutex);

	if (file->f_flags & FASYNC)
		comedi_fasync(-1, file, 0);

	return 0;
}
