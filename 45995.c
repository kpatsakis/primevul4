static unsigned int comedi_poll(struct file *file, poll_table * wait)
{
	unsigned int mask = 0;
	const unsigned minor = iminor(file->f_dentry->d_inode);
	struct comedi_device_file_info *dev_file_info =
	    comedi_get_device_file_info(minor);
	struct comedi_device *dev = dev_file_info->device;
	struct comedi_subdevice *read_subdev;
	struct comedi_subdevice *write_subdev;

	mutex_lock(&dev->mutex);
	if (!dev->attached) {
		DPRINTK("no driver configured on comedi%i\n", dev->minor);
		mutex_unlock(&dev->mutex);
		return 0;
	}

	mask = 0;
	read_subdev = comedi_get_read_subdevice(dev_file_info);
	if (read_subdev) {
		poll_wait(file, &read_subdev->async->wait_head, wait);
		if (!read_subdev->busy
		    || comedi_buf_read_n_available(read_subdev->async) > 0
		    || !(comedi_get_subdevice_runflags(read_subdev) &
			 SRF_RUNNING)) {
			mask |= POLLIN | POLLRDNORM;
		}
	}
	write_subdev = comedi_get_write_subdevice(dev_file_info);
	if (write_subdev) {
		poll_wait(file, &write_subdev->async->wait_head, wait);
		comedi_buf_write_alloc(write_subdev->async,
				       write_subdev->async->prealloc_bufsz);
		if (!write_subdev->busy
		    || !(comedi_get_subdevice_runflags(write_subdev) &
			 SRF_RUNNING)
		    || comedi_buf_write_n_allocated(write_subdev->async) >=
		    bytes_per_sample(write_subdev->async->subdevice)) {
			mask |= POLLOUT | POLLWRNORM;
		}
	}

	mutex_unlock(&dev->mutex);
	return mask;
}
