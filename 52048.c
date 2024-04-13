static ssize_t usbdev_read(struct file *file, char __user *buf, size_t nbytes,
			   loff_t *ppos)
{
	struct usb_dev_state *ps = file->private_data;
	struct usb_device *dev = ps->dev;
	ssize_t ret = 0;
	unsigned len;
	loff_t pos;
	int i;

	pos = *ppos;
	usb_lock_device(dev);
	if (!connected(ps)) {
		ret = -ENODEV;
		goto err;
	} else if (pos < 0) {
		ret = -EINVAL;
		goto err;
	}

	if (pos < sizeof(struct usb_device_descriptor)) {
		/* 18 bytes - fits on the stack */
		struct usb_device_descriptor temp_desc;

		memcpy(&temp_desc, &dev->descriptor, sizeof(dev->descriptor));
		le16_to_cpus(&temp_desc.bcdUSB);
		le16_to_cpus(&temp_desc.idVendor);
		le16_to_cpus(&temp_desc.idProduct);
		le16_to_cpus(&temp_desc.bcdDevice);

		len = sizeof(struct usb_device_descriptor) - pos;
		if (len > nbytes)
			len = nbytes;
		if (copy_to_user(buf, ((char *)&temp_desc) + pos, len)) {
			ret = -EFAULT;
			goto err;
		}

		*ppos += len;
		buf += len;
		nbytes -= len;
		ret += len;
	}

	pos = sizeof(struct usb_device_descriptor);
	for (i = 0; nbytes && i < dev->descriptor.bNumConfigurations; i++) {
		struct usb_config_descriptor *config =
			(struct usb_config_descriptor *)dev->rawdescriptors[i];
		unsigned int length = le16_to_cpu(config->wTotalLength);

		if (*ppos < pos + length) {

			/* The descriptor may claim to be longer than it
			 * really is.  Here is the actual allocated length. */
			unsigned alloclen =
				le16_to_cpu(dev->config[i].desc.wTotalLength);

			len = length - (*ppos - pos);
			if (len > nbytes)
				len = nbytes;

			/* Simply don't write (skip over) unallocated parts */
			if (alloclen > (*ppos - pos)) {
				alloclen -= (*ppos - pos);
				if (copy_to_user(buf,
				    dev->rawdescriptors[i] + (*ppos - pos),
				    min(len, alloclen))) {
					ret = -EFAULT;
					goto err;
				}
			}

			*ppos += len;
			buf += len;
			nbytes -= len;
			ret += len;
		}

		pos += length;
	}

err:
	usb_unlock_device(dev);
	return ret;
}
