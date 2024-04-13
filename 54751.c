static int usbhid_start(struct hid_device *hid)
{
	struct usb_interface *intf = to_usb_interface(hid->dev.parent);
	struct usb_host_interface *interface = intf->cur_altsetting;
	struct usb_device *dev = interface_to_usbdev(intf);
	struct usbhid_device *usbhid = hid->driver_data;
	unsigned int n, insize = 0;
	int ret;

	clear_bit(HID_DISCONNECTED, &usbhid->iofl);

	usbhid->bufsize = HID_MIN_BUFFER_SIZE;
	hid_find_max_report(hid, HID_INPUT_REPORT, &usbhid->bufsize);
	hid_find_max_report(hid, HID_OUTPUT_REPORT, &usbhid->bufsize);
	hid_find_max_report(hid, HID_FEATURE_REPORT, &usbhid->bufsize);

	if (usbhid->bufsize > HID_MAX_BUFFER_SIZE)
		usbhid->bufsize = HID_MAX_BUFFER_SIZE;

	hid_find_max_report(hid, HID_INPUT_REPORT, &insize);

	if (insize > HID_MAX_BUFFER_SIZE)
		insize = HID_MAX_BUFFER_SIZE;

	if (hid_alloc_buffers(dev, hid)) {
		ret = -ENOMEM;
		goto fail;
	}

	for (n = 0; n < interface->desc.bNumEndpoints; n++) {
		struct usb_endpoint_descriptor *endpoint;
		int pipe;
		int interval;

		endpoint = &interface->endpoint[n].desc;
		if (!usb_endpoint_xfer_int(endpoint))
			continue;

		interval = endpoint->bInterval;

		/* Some vendors give fullspeed interval on highspeed devides */
		if (hid->quirks & HID_QUIRK_FULLSPEED_INTERVAL &&
		    dev->speed == USB_SPEED_HIGH) {
			interval = fls(endpoint->bInterval*8);
			pr_info("%s: Fixing fullspeed to highspeed interval: %d -> %d\n",
				hid->name, endpoint->bInterval, interval);
		}

		/* Change the polling interval of mice and joysticks. */
		switch (hid->collection->usage) {
		case HID_GD_MOUSE:
			if (hid_mousepoll_interval > 0)
				interval = hid_mousepoll_interval;
			break;
		case HID_GD_JOYSTICK:
			if (hid_jspoll_interval > 0)
				interval = hid_jspoll_interval;
			break;
		}

		ret = -ENOMEM;
		if (usb_endpoint_dir_in(endpoint)) {
			if (usbhid->urbin)
				continue;
			if (!(usbhid->urbin = usb_alloc_urb(0, GFP_KERNEL)))
				goto fail;
			pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
			usb_fill_int_urb(usbhid->urbin, dev, pipe, usbhid->inbuf, insize,
					 hid_irq_in, hid, interval);
			usbhid->urbin->transfer_dma = usbhid->inbuf_dma;
			usbhid->urbin->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		} else {
			if (usbhid->urbout)
				continue;
			if (!(usbhid->urbout = usb_alloc_urb(0, GFP_KERNEL)))
				goto fail;
			pipe = usb_sndintpipe(dev, endpoint->bEndpointAddress);
			usb_fill_int_urb(usbhid->urbout, dev, pipe, usbhid->outbuf, 0,
					 hid_irq_out, hid, interval);
			usbhid->urbout->transfer_dma = usbhid->outbuf_dma;
			usbhid->urbout->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		}
	}

	usbhid->urbctrl = usb_alloc_urb(0, GFP_KERNEL);
	if (!usbhid->urbctrl) {
		ret = -ENOMEM;
		goto fail;
	}

	usb_fill_control_urb(usbhid->urbctrl, dev, 0, (void *) usbhid->cr,
			     usbhid->ctrlbuf, 1, hid_ctrl, hid);
	usbhid->urbctrl->transfer_dma = usbhid->ctrlbuf_dma;
	usbhid->urbctrl->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	set_bit(HID_STARTED, &usbhid->iofl);

	if (hid->quirks & HID_QUIRK_ALWAYS_POLL) {
		ret = usb_autopm_get_interface(usbhid->intf);
		if (ret)
			goto fail;
		set_bit(HID_IN_POLLING, &usbhid->iofl);
		usbhid->intf->needs_remote_wakeup = 1;
		ret = hid_start_in(hid);
		if (ret) {
			dev_err(&hid->dev,
				"failed to start in urb: %d\n", ret);
		}
		usb_autopm_put_interface(usbhid->intf);
	}

	/* Some keyboards don't work until their LEDs have been set.
	 * Since BIOSes do set the LEDs, it must be safe for any device
	 * that supports the keyboard boot protocol.
	 * In addition, enable remote wakeup by default for all keyboard
	 * devices supporting the boot protocol.
	 */
	if (interface->desc.bInterfaceSubClass == USB_INTERFACE_SUBCLASS_BOOT &&
			interface->desc.bInterfaceProtocol ==
				USB_INTERFACE_PROTOCOL_KEYBOARD) {
		usbhid_set_leds(hid);
		device_set_wakeup_enable(&dev->dev, 1);
	}
	return 0;

fail:
	usb_free_urb(usbhid->urbin);
	usb_free_urb(usbhid->urbout);
	usb_free_urb(usbhid->urbctrl);
	usbhid->urbin = NULL;
	usbhid->urbout = NULL;
	usbhid->urbctrl = NULL;
	hid_free_buffers(dev, hid);
	return ret;
}
