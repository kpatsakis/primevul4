static int wdm_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	int rv = -EINVAL;
	struct usb_host_interface *iface;
	struct usb_endpoint_descriptor *ep;
	struct usb_cdc_dmm_desc *dmhd;
	u8 *buffer = intf->altsetting->extra;
	int buflen = intf->altsetting->extralen;
	u16 maxcom = WDM_DEFAULT_BUFSIZE;

	if (!buffer)
		goto err;
	while (buflen > 2) {
		if (buffer[1] != USB_DT_CS_INTERFACE) {
			dev_err(&intf->dev, "skipping garbage\n");
			goto next_desc;
		}

		switch (buffer[2]) {
		case USB_CDC_HEADER_TYPE:
			break;
		case USB_CDC_DMM_TYPE:
			dmhd = (struct usb_cdc_dmm_desc *)buffer;
			maxcom = le16_to_cpu(dmhd->wMaxCommand);
			dev_dbg(&intf->dev,
				"Finding maximum buffer length: %d", maxcom);
			break;
		default:
			dev_err(&intf->dev,
				"Ignoring extra header, type %d, length %d\n",
				buffer[2], buffer[0]);
			break;
		}
next_desc:
		buflen -= buffer[0];
		buffer += buffer[0];
	}

	iface = intf->cur_altsetting;
	if (iface->desc.bNumEndpoints != 1)
		goto err;
	ep = &iface->endpoint[0].desc;

	rv = wdm_create(intf, ep, maxcom, &wdm_manage_power);

err:
	return rv;
}
