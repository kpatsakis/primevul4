cdc_ncm_find_endpoints(struct usbnet *dev, struct usb_interface *intf)
{
	struct usb_host_endpoint *e, *in = NULL, *out = NULL;
	u8 ep;

	for (ep = 0; ep < intf->cur_altsetting->desc.bNumEndpoints; ep++) {

		e = intf->cur_altsetting->endpoint + ep;
		switch (e->desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
		case USB_ENDPOINT_XFER_INT:
			if (usb_endpoint_dir_in(&e->desc)) {
				if (!dev->status)
					dev->status = e;
			}
			break;

		case USB_ENDPOINT_XFER_BULK:
			if (usb_endpoint_dir_in(&e->desc)) {
				if (!in)
					in = e;
			} else {
				if (!out)
					out = e;
			}
			break;

		default:
			break;
		}
	}
	if (in && !dev->in)
		dev->in = usb_rcvbulkpipe(dev->udev,
					  in->desc.bEndpointAddress &
					  USB_ENDPOINT_NUMBER_MASK);
	if (out && !dev->out)
		dev->out = usb_sndbulkpipe(dev->udev,
					   out->desc.bEndpointAddress &
					   USB_ENDPOINT_NUMBER_MASK);
}
