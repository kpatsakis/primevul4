static int resize_async_buffer(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_async *async, unsigned new_size)
{
	int retval;

	if (new_size > async->max_bufsize)
		return -EPERM;

	if (s->busy) {
		DPRINTK("subdevice is busy, cannot resize buffer\n");
		return -EBUSY;
	}
	if (async->mmap_count) {
		DPRINTK("subdevice is mmapped, cannot resize buffer\n");
		return -EBUSY;
	}

	if (!async->prealloc_buf)
		return -EINVAL;

	/* make sure buffer is an integral number of pages
	 * (we round up) */
	new_size = (new_size + PAGE_SIZE - 1) & PAGE_MASK;

	retval = comedi_buf_alloc(dev, s, new_size);
	if (retval < 0)
		return retval;

	if (s->buf_change) {
		retval = s->buf_change(dev, s, new_size);
		if (retval < 0)
			return retval;
	}

	DPRINTK("comedi%i subd %d buffer resized to %i bytes\n",
		dev->minor, (int)(s - dev->subdevices), async->prealloc_bufsz);
	return 0;
}
