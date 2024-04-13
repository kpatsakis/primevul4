alloc_sglist(int nents, int max, int vary, struct usbtest_dev *dev, int pipe)
{
	struct scatterlist	*sg;
	unsigned int		n_size = 0;
	unsigned		i;
	unsigned		size = max;
	unsigned		maxpacket =
		get_maxpacket(interface_to_usbdev(dev->intf), pipe);

	if (max == 0)
		return NULL;

	sg = kmalloc_array(nents, sizeof(*sg), GFP_KERNEL);
	if (!sg)
		return NULL;
	sg_init_table(sg, nents);

	for (i = 0; i < nents; i++) {
		char		*buf;
		unsigned	j;

		buf = kzalloc(size, GFP_KERNEL);
		if (!buf) {
			free_sglist(sg, i);
			return NULL;
		}

		/* kmalloc pages are always physically contiguous! */
		sg_set_buf(&sg[i], buf, size);

		switch (pattern) {
		case 0:
			/* already zeroed */
			break;
		case 1:
			for (j = 0; j < size; j++)
				*buf++ = (u8) (((j + n_size) % maxpacket) % 63);
			n_size += size;
			break;
		}

		if (vary) {
			size += vary;
			size %= max;
			if (size == 0)
				size = (vary < max) ? vary : max;
		}
	}

	return sg;
}
