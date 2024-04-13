static int do_bufinfo_ioctl(struct comedi_device *dev,
			    struct comedi_bufinfo __user *arg, void *file)
{
	struct comedi_bufinfo bi;
	struct comedi_subdevice *s;
	struct comedi_async *async;

	if (copy_from_user(&bi, arg, sizeof(struct comedi_bufinfo)))
		return -EFAULT;

	if (bi.subdevice >= dev->n_subdevices || bi.subdevice < 0)
		return -EINVAL;

	s = dev->subdevices + bi.subdevice;

	if (s->lock && s->lock != file)
		return -EACCES;

	async = s->async;

	if (!async) {
		DPRINTK("subdevice does not have async capability\n");
		bi.buf_write_ptr = 0;
		bi.buf_read_ptr = 0;
		bi.buf_write_count = 0;
		bi.buf_read_count = 0;
		bi.bytes_read = 0;
		bi.bytes_written = 0;
		goto copyback;
	}
	if (!s->busy) {
		bi.bytes_read = 0;
		bi.bytes_written = 0;
		goto copyback_position;
	}
	if (s->busy != file)
		return -EACCES;

	if (bi.bytes_read && (s->subdev_flags & SDF_CMD_READ)) {
		bi.bytes_read = comedi_buf_read_alloc(async, bi.bytes_read);
		comedi_buf_read_free(async, bi.bytes_read);

		if (!(comedi_get_subdevice_runflags(s) & (SRF_ERROR |
							  SRF_RUNNING))
		    && async->buf_write_count == async->buf_read_count) {
			do_become_nonbusy(dev, s);
		}
	}

	if (bi.bytes_written && (s->subdev_flags & SDF_CMD_WRITE)) {
		bi.bytes_written =
		    comedi_buf_write_alloc(async, bi.bytes_written);
		comedi_buf_write_free(async, bi.bytes_written);
	}

copyback_position:
	bi.buf_write_count = async->buf_write_count;
	bi.buf_write_ptr = async->buf_write_ptr;
	bi.buf_read_count = async->buf_read_count;
	bi.buf_read_ptr = async->buf_read_ptr;

copyback:
	if (copy_to_user(arg, &bi, sizeof(struct comedi_bufinfo)))
		return -EFAULT;

	return 0;
}
