int usb_sg_init(struct usb_sg_request *io, struct usb_device *dev,
		unsigned pipe, unsigned	period, struct scatterlist *sg,
		int nents, size_t length, gfp_t mem_flags)
{
	int i;
	int urb_flags;
	int use_sg;

	if (!io || !dev || !sg
			|| usb_pipecontrol(pipe)
			|| usb_pipeisoc(pipe)
			|| nents <= 0)
		return -EINVAL;

	spin_lock_init(&io->lock);
	io->dev = dev;
	io->pipe = pipe;

	if (dev->bus->sg_tablesize > 0) {
		use_sg = true;
		io->entries = 1;
	} else {
		use_sg = false;
		io->entries = nents;
	}

	/* initialize all the urbs we'll use */
	io->urbs = kmalloc(io->entries * sizeof(*io->urbs), mem_flags);
	if (!io->urbs)
		goto nomem;

	urb_flags = URB_NO_INTERRUPT;
	if (usb_pipein(pipe))
		urb_flags |= URB_SHORT_NOT_OK;

	for_each_sg(sg, sg, io->entries, i) {
		struct urb *urb;
		unsigned len;

		urb = usb_alloc_urb(0, mem_flags);
		if (!urb) {
			io->entries = i;
			goto nomem;
		}
		io->urbs[i] = urb;

		urb->dev = NULL;
		urb->pipe = pipe;
		urb->interval = period;
		urb->transfer_flags = urb_flags;
		urb->complete = sg_complete;
		urb->context = io;
		urb->sg = sg;

		if (use_sg) {
			/* There is no single transfer buffer */
			urb->transfer_buffer = NULL;
			urb->num_sgs = nents;

			/* A length of zero means transfer the whole sg list */
			len = length;
			if (len == 0) {
				struct scatterlist	*sg2;
				int			j;

				for_each_sg(sg, sg2, nents, j)
					len += sg2->length;
			}
		} else {
			/*
			 * Some systems can't use DMA; they use PIO instead.
			 * For their sakes, transfer_buffer is set whenever
			 * possible.
			 */
			if (!PageHighMem(sg_page(sg)))
				urb->transfer_buffer = sg_virt(sg);
			else
				urb->transfer_buffer = NULL;

			len = sg->length;
			if (length) {
				len = min_t(size_t, len, length);
				length -= len;
				if (length == 0)
					io->entries = i + 1;
			}
		}
		urb->transfer_buffer_length = len;
	}
	io->urbs[--i]->transfer_flags &= ~URB_NO_INTERRUPT;

	/* transaction state */
	io->count = io->entries;
	io->status = 0;
	io->bytes = 0;
	init_completion(&io->complete);
	return 0;

nomem:
	sg_clean(io);
	return -ENOMEM;
}
