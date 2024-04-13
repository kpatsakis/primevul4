test_queue(struct usbtest_dev *dev, struct usbtest_param_32 *param,
		int pipe, struct usb_endpoint_descriptor *desc, unsigned offset)
{
	struct transfer_context	context;
	struct usb_device	*udev;
	unsigned		i;
	unsigned long		packets = 0;
	int			status = 0;
	struct urb		*urbs[param->sglen];

	if (!param->sglen || param->iterations > UINT_MAX / param->sglen)
		return -EINVAL;

	memset(&context, 0, sizeof(context));
	context.count = param->iterations * param->sglen;
	context.dev = dev;
	context.is_iso = !!desc;
	init_completion(&context.done);
	spin_lock_init(&context.lock);

	udev = testdev_to_usbdev(dev);

	for (i = 0; i < param->sglen; i++) {
		if (context.is_iso)
			urbs[i] = iso_alloc_urb(udev, pipe, desc,
					param->length, offset);
		else
			urbs[i] = complicated_alloc_urb(udev, pipe,
					param->length, 0);

		if (!urbs[i]) {
			status = -ENOMEM;
			goto fail;
		}
		packets += urbs[i]->number_of_packets;
		urbs[i]->context = &context;
	}
	packets *= param->iterations;

	if (context.is_iso) {
		dev_info(&dev->intf->dev,
			"iso period %d %sframes, wMaxPacket %d, transactions: %d\n",
			1 << (desc->bInterval - 1),
			(udev->speed == USB_SPEED_HIGH) ? "micro" : "",
			usb_endpoint_maxp(desc),
			usb_endpoint_maxp_mult(desc));

		dev_info(&dev->intf->dev,
			"total %lu msec (%lu packets)\n",
			(packets * (1 << (desc->bInterval - 1)))
				/ ((udev->speed == USB_SPEED_HIGH) ? 8 : 1),
			packets);
	}

	spin_lock_irq(&context.lock);
	for (i = 0; i < param->sglen; i++) {
		++context.pending;
		status = usb_submit_urb(urbs[i], GFP_ATOMIC);
		if (status < 0) {
			ERROR(dev, "submit iso[%d], error %d\n", i, status);
			if (i == 0) {
				spin_unlock_irq(&context.lock);
				goto fail;
			}

			simple_free_urb(urbs[i]);
			urbs[i] = NULL;
			context.pending--;
			context.submit_error = 1;
			break;
		}
	}
	spin_unlock_irq(&context.lock);

	wait_for_completion(&context.done);

	for (i = 0; i < param->sglen; i++) {
		if (urbs[i])
			simple_free_urb(urbs[i]);
	}
	/*
	 * Isochronous transfers are expected to fail sometimes.  As an
	 * arbitrary limit, we will report an error if any submissions
	 * fail or if the transfer failure rate is > 10%.
	 */
	if (status != 0)
		;
	else if (context.submit_error)
		status = -EACCES;
	else if (context.errors >
			(context.is_iso ? context.packet_count / 10 : 0))
		status = -EIO;
	return status;

fail:
	for (i = 0; i < param->sglen; i++) {
		if (urbs[i])
			simple_free_urb(urbs[i]);
	}
	return status;
}
