static struct usb_host_endpoint *ep_to_host_endpoint(struct usb_device *dev,
						     unsigned char ep)
{
	if (ep & USB_ENDPOINT_DIR_MASK)
		return dev->ep_in[ep & USB_ENDPOINT_NUMBER_MASK];
	else
		return dev->ep_out[ep & USB_ENDPOINT_NUMBER_MASK];
}
