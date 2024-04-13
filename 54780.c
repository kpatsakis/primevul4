test_ctrl_queue(struct usbtest_dev *dev, struct usbtest_param_32 *param)
{
	struct usb_device	*udev = testdev_to_usbdev(dev);
	struct urb		**urb;
	struct ctrl_ctx		context;
	int			i;

	if (param->sglen == 0 || param->iterations > UINT_MAX / param->sglen)
		return -EOPNOTSUPP;

	spin_lock_init(&context.lock);
	context.dev = dev;
	init_completion(&context.complete);
	context.count = param->sglen * param->iterations;
	context.pending = 0;
	context.status = -ENOMEM;
	context.param = param;
	context.last = -1;

	/* allocate and init the urbs we'll queue.
	 * as with bulk/intr sglists, sglen is the queue depth; it also
	 * controls which subtests run (more tests than sglen) or rerun.
	 */
	urb = kcalloc(param->sglen, sizeof(struct urb *), GFP_KERNEL);
	if (!urb)
		return -ENOMEM;
	for (i = 0; i < param->sglen; i++) {
		int			pipe = usb_rcvctrlpipe(udev, 0);
		unsigned		len;
		struct urb		*u;
		struct usb_ctrlrequest	req;
		struct subcase		*reqp;

		/* sign of this variable means:
		 *  -: tested code must return this (negative) error code
		 *  +: tested code may return this (negative too) error code
		 */
		int			expected = 0;

		/* requests here are mostly expected to succeed on any
		 * device, but some are chosen to trigger protocol stalls
		 * or short reads.
		 */
		memset(&req, 0, sizeof(req));
		req.bRequest = USB_REQ_GET_DESCRIPTOR;
		req.bRequestType = USB_DIR_IN|USB_RECIP_DEVICE;

		switch (i % NUM_SUBCASES) {
		case 0:		/* get device descriptor */
			req.wValue = cpu_to_le16(USB_DT_DEVICE << 8);
			len = sizeof(struct usb_device_descriptor);
			break;
		case 1:		/* get first config descriptor (only) */
			req.wValue = cpu_to_le16((USB_DT_CONFIG << 8) | 0);
			len = sizeof(struct usb_config_descriptor);
			break;
		case 2:		/* get altsetting (OFTEN STALLS) */
			req.bRequest = USB_REQ_GET_INTERFACE;
			req.bRequestType = USB_DIR_IN|USB_RECIP_INTERFACE;
			/* index = 0 means first interface */
			len = 1;
			expected = EPIPE;
			break;
		case 3:		/* get interface status */
			req.bRequest = USB_REQ_GET_STATUS;
			req.bRequestType = USB_DIR_IN|USB_RECIP_INTERFACE;
			/* interface 0 */
			len = 2;
			break;
		case 4:		/* get device status */
			req.bRequest = USB_REQ_GET_STATUS;
			req.bRequestType = USB_DIR_IN|USB_RECIP_DEVICE;
			len = 2;
			break;
		case 5:		/* get device qualifier (MAY STALL) */
			req.wValue = cpu_to_le16 (USB_DT_DEVICE_QUALIFIER << 8);
			len = sizeof(struct usb_qualifier_descriptor);
			if (udev->speed != USB_SPEED_HIGH)
				expected = EPIPE;
			break;
		case 6:		/* get first config descriptor, plus interface */
			req.wValue = cpu_to_le16((USB_DT_CONFIG << 8) | 0);
			len = sizeof(struct usb_config_descriptor);
			len += sizeof(struct usb_interface_descriptor);
			break;
		case 7:		/* get interface descriptor (ALWAYS STALLS) */
			req.wValue = cpu_to_le16 (USB_DT_INTERFACE << 8);
			/* interface == 0 */
			len = sizeof(struct usb_interface_descriptor);
			expected = -EPIPE;
			break;
		/* NOTE: two consecutive stalls in the queue here.
		 *  that tests fault recovery a bit more aggressively. */
		case 8:		/* clear endpoint halt (MAY STALL) */
			req.bRequest = USB_REQ_CLEAR_FEATURE;
			req.bRequestType = USB_RECIP_ENDPOINT;
			/* wValue 0 == ep halt */
			/* wIndex 0 == ep0 (shouldn't halt!) */
			len = 0;
			pipe = usb_sndctrlpipe(udev, 0);
			expected = EPIPE;
			break;
		case 9:		/* get endpoint status */
			req.bRequest = USB_REQ_GET_STATUS;
			req.bRequestType = USB_DIR_IN|USB_RECIP_ENDPOINT;
			/* endpoint 0 */
			len = 2;
			break;
		case 10:	/* trigger short read (EREMOTEIO) */
			req.wValue = cpu_to_le16((USB_DT_CONFIG << 8) | 0);
			len = 1024;
			expected = -EREMOTEIO;
			break;
		/* NOTE: two consecutive _different_ faults in the queue. */
		case 11:	/* get endpoint descriptor (ALWAYS STALLS) */
			req.wValue = cpu_to_le16(USB_DT_ENDPOINT << 8);
			/* endpoint == 0 */
			len = sizeof(struct usb_interface_descriptor);
			expected = EPIPE;
			break;
		/* NOTE: sometimes even a third fault in the queue! */
		case 12:	/* get string 0 descriptor (MAY STALL) */
			req.wValue = cpu_to_le16(USB_DT_STRING << 8);
			/* string == 0, for language IDs */
			len = sizeof(struct usb_interface_descriptor);
			/* may succeed when > 4 languages */
			expected = EREMOTEIO;	/* or EPIPE, if no strings */
			break;
		case 13:	/* short read, resembling case 10 */
			req.wValue = cpu_to_le16((USB_DT_CONFIG << 8) | 0);
			/* last data packet "should" be DATA1, not DATA0 */
			if (udev->speed == USB_SPEED_SUPER)
				len = 1024 - 512;
			else
				len = 1024 - udev->descriptor.bMaxPacketSize0;
			expected = -EREMOTEIO;
			break;
		case 14:	/* short read; try to fill the last packet */
			req.wValue = cpu_to_le16((USB_DT_DEVICE << 8) | 0);
			/* device descriptor size == 18 bytes */
			len = udev->descriptor.bMaxPacketSize0;
			if (udev->speed == USB_SPEED_SUPER)
				len = 512;
			switch (len) {
			case 8:
				len = 24;
				break;
			case 16:
				len = 32;
				break;
			}
			expected = -EREMOTEIO;
			break;
		case 15:
			req.wValue = cpu_to_le16(USB_DT_BOS << 8);
			if (udev->bos)
				len = le16_to_cpu(udev->bos->desc->wTotalLength);
			else
				len = sizeof(struct usb_bos_descriptor);
			if (le16_to_cpu(udev->descriptor.bcdUSB) < 0x0201)
				expected = -EPIPE;
			break;
		default:
			ERROR(dev, "bogus number of ctrl queue testcases!\n");
			context.status = -EINVAL;
			goto cleanup;
		}
		req.wLength = cpu_to_le16(len);
		urb[i] = u = simple_alloc_urb(udev, pipe, len, 0);
		if (!u)
			goto cleanup;

		reqp = kmalloc(sizeof(*reqp), GFP_KERNEL);
		if (!reqp)
			goto cleanup;
		reqp->setup = req;
		reqp->number = i % NUM_SUBCASES;
		reqp->expected = expected;
		u->setup_packet = (char *) &reqp->setup;

		u->context = &context;
		u->complete = ctrl_complete;
	}

	/* queue the urbs */
	context.urb = urb;
	spin_lock_irq(&context.lock);
	for (i = 0; i < param->sglen; i++) {
		context.status = usb_submit_urb(urb[i], GFP_ATOMIC);
		if (context.status != 0) {
			ERROR(dev, "can't submit urb[%d], status %d\n",
					i, context.status);
			context.count = context.pending;
			break;
		}
		context.pending++;
	}
	spin_unlock_irq(&context.lock);

	/* FIXME  set timer and time out; provide a disconnect hook */

	/* wait for the last one to complete */
	if (context.pending > 0)
		wait_for_completion(&context.complete);

cleanup:
	for (i = 0; i < param->sglen; i++) {
		if (!urb[i])
			continue;
		urb[i]->dev = udev;
		kfree(urb[i]->setup_packet);
		simple_free_urb(urb[i]);
	}
	kfree(urb);
	return context.status;
}
