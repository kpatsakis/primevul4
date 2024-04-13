static int ch9_postconfig(struct usbtest_dev *dev)
{
	struct usb_interface	*iface = dev->intf;
	struct usb_device	*udev = interface_to_usbdev(iface);
	int			i, alt, retval;

	/* [9.2.3] if there's more than one altsetting, we need to be able to
	 * set and get each one.  mostly trusts the descriptors from usbcore.
	 */
	for (i = 0; i < iface->num_altsetting; i++) {

		/* 9.2.3 constrains the range here */
		alt = iface->altsetting[i].desc.bAlternateSetting;
		if (alt < 0 || alt >= iface->num_altsetting) {
			dev_err(&iface->dev,
					"invalid alt [%d].bAltSetting = %d\n",
					i, alt);
		}

		/* [real world] get/set unimplemented if there's only one */
		if (realworld && iface->num_altsetting == 1)
			continue;

		/* [9.4.10] set_interface */
		retval = set_altsetting(dev, alt);
		if (retval) {
			dev_err(&iface->dev, "can't set_interface = %d, %d\n",
					alt, retval);
			return retval;
		}

		/* [9.4.4] get_interface always works */
		retval = get_altsetting(dev);
		if (retval != alt) {
			dev_err(&iface->dev, "get alt should be %d, was %d\n",
					alt, retval);
			return (retval < 0) ? retval : -EDOM;
		}

	}

	/* [real world] get_config unimplemented if there's only one */
	if (!realworld || udev->descriptor.bNumConfigurations != 1) {
		int	expected = udev->actconfig->desc.bConfigurationValue;

		/* [9.4.2] get_configuration always works
		 * ... although some cheap devices (like one TI Hub I've got)
		 * won't return config descriptors except before set_config.
		 */
		retval = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
				USB_REQ_GET_CONFIGURATION,
				USB_DIR_IN | USB_RECIP_DEVICE,
				0, 0, dev->buf, 1, USB_CTRL_GET_TIMEOUT);
		if (retval != 1 || dev->buf[0] != expected) {
			dev_err(&iface->dev, "get config --> %d %d (1 %d)\n",
				retval, dev->buf[0], expected);
			return (retval < 0) ? retval : -EDOM;
		}
	}

	/* there's always [9.4.3] a device descriptor [9.6.1] */
	retval = usb_get_descriptor(udev, USB_DT_DEVICE, 0,
			dev->buf, sizeof(udev->descriptor));
	if (retval != sizeof(udev->descriptor)) {
		dev_err(&iface->dev, "dev descriptor --> %d\n", retval);
		return (retval < 0) ? retval : -EDOM;
	}

	/*
	 * there's always [9.4.3] a bos device descriptor [9.6.2] in USB
	 * 3.0 spec
	 */
	if (le16_to_cpu(udev->descriptor.bcdUSB) >= 0x0210) {
		struct usb_bos_descriptor *bos = NULL;
		struct usb_dev_cap_header *header = NULL;
		unsigned total, num, length;
		u8 *buf;

		retval = usb_get_descriptor(udev, USB_DT_BOS, 0, dev->buf,
				sizeof(*udev->bos->desc));
		if (retval != sizeof(*udev->bos->desc)) {
			dev_err(&iface->dev, "bos descriptor --> %d\n", retval);
			return (retval < 0) ? retval : -EDOM;
		}

		bos = (struct usb_bos_descriptor *)dev->buf;
		total = le16_to_cpu(bos->wTotalLength);
		num = bos->bNumDeviceCaps;

		if (total > TBUF_SIZE)
			total = TBUF_SIZE;

		/*
		 * get generic device-level capability descriptors [9.6.2]
		 * in USB 3.0 spec
		 */
		retval = usb_get_descriptor(udev, USB_DT_BOS, 0, dev->buf,
				total);
		if (retval != total) {
			dev_err(&iface->dev, "bos descriptor set --> %d\n",
					retval);
			return (retval < 0) ? retval : -EDOM;
		}

		length = sizeof(*udev->bos->desc);
		buf = dev->buf;
		for (i = 0; i < num; i++) {
			buf += length;
			if (buf + sizeof(struct usb_dev_cap_header) >
					dev->buf + total)
				break;

			header = (struct usb_dev_cap_header *)buf;
			length = header->bLength;

			if (header->bDescriptorType !=
					USB_DT_DEVICE_CAPABILITY) {
				dev_warn(&udev->dev, "not device capability descriptor, skip\n");
				continue;
			}

			switch (header->bDevCapabilityType) {
			case USB_CAP_TYPE_EXT:
				if (buf + USB_DT_USB_EXT_CAP_SIZE >
						dev->buf + total ||
						!is_good_ext(dev, buf)) {
					dev_err(&iface->dev, "bogus usb 2.0 extension descriptor\n");
					return -EDOM;
				}
				break;
			case USB_SS_CAP_TYPE:
				if (buf + USB_DT_USB_SS_CAP_SIZE >
						dev->buf + total ||
						!is_good_ss_cap(dev, buf)) {
					dev_err(&iface->dev, "bogus superspeed device capability descriptor\n");
					return -EDOM;
				}
				break;
			case CONTAINER_ID_TYPE:
				if (buf + USB_DT_USB_SS_CONTN_ID_SIZE >
						dev->buf + total ||
						!is_good_con_id(dev, buf)) {
					dev_err(&iface->dev, "bogus container id descriptor\n");
					return -EDOM;
				}
				break;
			default:
				break;
			}
		}
	}

	/* there's always [9.4.3] at least one config descriptor [9.6.3] */
	for (i = 0; i < udev->descriptor.bNumConfigurations; i++) {
		retval = usb_get_descriptor(udev, USB_DT_CONFIG, i,
				dev->buf, TBUF_SIZE);
		if (!is_good_config(dev, retval)) {
			dev_err(&iface->dev,
					"config [%d] descriptor --> %d\n",
					i, retval);
			return (retval < 0) ? retval : -EDOM;
		}

		/* FIXME cross-checking udev->config[i] to make sure usbcore
		 * parsed it right (etc) would be good testing paranoia
		 */
	}

	/* and sometimes [9.2.6.6] speed dependent descriptors */
	if (le16_to_cpu(udev->descriptor.bcdUSB) == 0x0200) {
		struct usb_qualifier_descriptor *d = NULL;

		/* device qualifier [9.6.2] */
		retval = usb_get_descriptor(udev,
				USB_DT_DEVICE_QUALIFIER, 0, dev->buf,
				sizeof(struct usb_qualifier_descriptor));
		if (retval == -EPIPE) {
			if (udev->speed == USB_SPEED_HIGH) {
				dev_err(&iface->dev,
						"hs dev qualifier --> %d\n",
						retval);
				return retval;
			}
			/* usb2.0 but not high-speed capable; fine */
		} else if (retval != sizeof(struct usb_qualifier_descriptor)) {
			dev_err(&iface->dev, "dev qualifier --> %d\n", retval);
			return (retval < 0) ? retval : -EDOM;
		} else
			d = (struct usb_qualifier_descriptor *) dev->buf;

		/* might not have [9.6.2] any other-speed configs [9.6.4] */
		if (d) {
			unsigned max = d->bNumConfigurations;
			for (i = 0; i < max; i++) {
				retval = usb_get_descriptor(udev,
					USB_DT_OTHER_SPEED_CONFIG, i,
					dev->buf, TBUF_SIZE);
				if (!is_good_config(dev, retval)) {
					dev_err(&iface->dev,
						"other speed config --> %d\n",
						retval);
					return (retval < 0) ? retval : -EDOM;
				}
			}
		}
	}
	/* FIXME fetch strings from at least the device descriptor */

	/* [9.4.5] get_status always works */
	retval = usb_get_status(udev, USB_RECIP_DEVICE, 0, dev->buf);
	if (retval) {
		dev_err(&iface->dev, "get dev status --> %d\n", retval);
		return retval;
	}

	/* FIXME configuration.bmAttributes says if we could try to set/clear
	 * the device's remote wakeup feature ... if we can, test that here
	 */

	retval = usb_get_status(udev, USB_RECIP_INTERFACE,
			iface->altsetting[0].desc.bInterfaceNumber, dev->buf);
	if (retval) {
		dev_err(&iface->dev, "get interface status --> %d\n", retval);
		return retval;
	}
	/* FIXME get status for each endpoint in the interface */

	return 0;
}
